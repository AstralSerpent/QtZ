/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// sv_bot.c

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"
#include "../botlib/botlib.h"
#include "../sys/sys_local.h"
#include "../sys/sys_loadlib.h"
#include "sv_local.h"

typedef struct bot_debugpoly_s
{
	int inuse;
	int color;
	int numPoints;
	vector3 points[128];
} bot_debugpoly_t;

static bot_debugpoly_t *debugpolygons;
int bot_maxdebugpolys;

botlib_export_t	*botlib_export = NULL;
static void *botlibLib = NULL;

int	bot_enable;


/*
==================
SV_BotAllocateClient
==================
*/
int SV_BotAllocateClient(void) {
	int			i;
	client_t	*cl;

	// find a client slot
	for ( i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++ ) {
		if ( cl->state == CS_FREE ) {
			break;
		}
	}

	if ( i == sv_maxclients->integer ) {
		return -1;
	}

	cl->gentity = SV_GentityNum( i );
	cl->gentity->s.number = i;
	cl->state = CS_ACTIVE;
	cl->lastPacketTime = svs.time;
	cl->netchan.remoteAddress.type = NA_BOT;
	cl->rate = 16384;

	return i;
}

/*
==================
SV_BotFreeClient
==================
*/
void SV_BotFreeClient( int clientNum ) {
	client_t	*cl;

	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		svi.Error( ERR_DROP, "SV_BotFreeClient: bad clientNum: %i", clientNum );
	}
	cl = &svs.clients[clientNum];
	cl->state = CS_FREE;
	cl->name[0] = 0;
	if ( cl->gentity ) {
		cl->gentity->r.svFlags &= ~SVF_BOT;
	}
}

/*
==================
BotDrawDebugPolygons
==================
*/
void BotDrawDebugPolygons(void (*drawPoly)(int color, int numPoints, vector3 *points), int value) {
	static cvar_t *bot_debug, *bot_groundonly, *bot_reachability, *bot_highlightarea;
	bot_debugpoly_t *poly;
	int i, parm0;

	if (!debugpolygons)
		return;
	//bot debugging
	if (!bot_debug) bot_debug = svi.Cvar_Get("bot_debug", "0", 0, NULL);
	//
	if (bot_enable && bot_debug->integer) {
		//show reachabilities
		if (!bot_reachability) bot_reachability = svi.Cvar_Get("bot_reachability", "0", 0, NULL);
		//show ground faces only
		if (!bot_groundonly) bot_groundonly = svi.Cvar_Get("bot_groundonly", "1", 0, NULL);
		//get the hightlight area
		if (!bot_highlightarea) bot_highlightarea = svi.Cvar_Get("bot_highlightarea", "0", 0, NULL);
		//
		parm0 = 0;
		if (svs.clients[0].lastUsercmd.buttons & BUTTON_ATTACK) parm0 |= 1;
		if (bot_reachability->integer) parm0 |= 2;
		if (bot_groundonly->integer) parm0 |= 4;
		botlib_export->BotLibVarSet("bot_highlightarea", bot_highlightarea->string);
		botlib_export->Test(parm0, NULL, &svs.clients[0].gentity->r.currentOrigin, &svs.clients[0].gentity->r.currentAngles);
	} //end if
	//draw all debug polys
	for (i = 0; i < bot_maxdebugpolys; i++) {
		poly = &debugpolygons[i];
		if (!poly->inuse) continue;
		drawPoly(poly->color, poly->numPoints, poly->points);
		//Com_Printf("poly %i, numpoints = %d\n", i, poly->numPoints);
	}
}

/*
==================
BotImport_Print
==================
*/
static __attribute__ ((format (printf, 2, 3))) void QDECL BotImport_Print(int type, char *fmt, ...)
{
	char str[2048];
	va_list ap;

	va_start(ap, fmt);
	Q_vsnprintf(str, sizeof(str), fmt, ap);
	va_end(ap);

	switch(type) {
		case PRT_MESSAGE: {
			Com_Printf("%s", str);
			break;
		}
		case PRT_WARNING: {
			Com_Printf(S_COLOR_YELLOW "Warning: %s", str);
			break;
		}
		case PRT_ERROR: {
			Com_Printf(S_COLOR_RED "Error: %s", str);
			break;
		}
		case PRT_FATAL: {
			Com_Printf(S_COLOR_RED "Fatal: %s", str);
			break;
		}
		case PRT_EXIT: {
			svi.Error(ERR_DROP, S_COLOR_RED "Exit: %s", str);
			break;
		}
		default: {
			Com_Printf("unknown print type\n");
			break;
		}
	}
}

/*
==================
BotImport_Trace
==================
*/
static void BotImport_Trace(bsp_trace_t *bsptrace, vector3 *start, vector3 *mins, vector3 *maxs, vector3 *end, int passent, int contentmask) {
	trace_t trace;

	SV_Trace(&trace, start, mins, maxs, end, passent, contentmask, qfalse);
	//copy the trace information
	bsptrace->allsolid = trace.allsolid;
	bsptrace->startsolid = trace.startsolid;
	bsptrace->fraction = trace.fraction;
	VectorCopy(&trace.endpos, &bsptrace->endpos);
	bsptrace->plane.dist = trace.plane.dist;
	VectorCopy(&trace.plane.normal, &bsptrace->plane.normal);
	bsptrace->plane.signbits = trace.plane.signbits;
	bsptrace->plane.type = trace.plane.type;
	bsptrace->surface.value = trace.surfaceFlags;
	bsptrace->ent = trace.entityNum;
	bsptrace->exp_dist = 0;
	bsptrace->sidenum = 0;
	bsptrace->contents = 0;
}

/*
==================
BotImport_EntityTrace
==================
*/
static void BotImport_EntityTrace(bsp_trace_t *bsptrace, vector3 *start, vector3 *mins, vector3 *maxs, vector3 *end, int entnum, int contentmask) {
	trace_t trace;

	SV_ClipToEntity(&trace, start, mins, maxs, end, entnum, contentmask, qfalse);
	//copy the trace information
	bsptrace->allsolid = trace.allsolid;
	bsptrace->startsolid = trace.startsolid;
	bsptrace->fraction = trace.fraction;
	VectorCopy(&trace.endpos, &bsptrace->endpos);
	bsptrace->plane.dist = trace.plane.dist;
	VectorCopy(&trace.plane.normal, &bsptrace->plane.normal);
	bsptrace->plane.signbits = trace.plane.signbits;
	bsptrace->plane.type = trace.plane.type;
	bsptrace->surface.value = trace.surfaceFlags;
	bsptrace->ent = trace.entityNum;
	bsptrace->exp_dist = 0;
	bsptrace->sidenum = 0;
	bsptrace->contents = 0;
}


/*
==================
BotImport_PointContents
==================
*/
static int BotImport_PointContents(vector3 *point) {
	return SV_PointContents(point, -1);
}

/*
==================
BotImport_inPVS
==================
*/
static int BotImport_inPVS(vector3 *p1, vector3 *p2) {
	return SV_InPVS (p1, p2);
}

/*
==================
BotImport_BSPEntityData
==================
*/
static char *BotImport_BSPEntityData(void) {
	return svi.CM_EntityString();
}

/*
==================
BotImport_BSPModelMinsMaxsOrigin
==================
*/
static void BotImport_BSPModelMinsMaxsOrigin(int modelnum, vector3 *angles, vector3 *outmins, vector3 *outmaxs, vector3 *origin) {
	clipHandle_t h;
	vector3 mins, maxs;
	float max;
	int	i;

	h = svi.CM_InlineModel(modelnum);
	svi.CM_ModelBounds(h, &mins, &maxs);
	//if the model is rotated
	if ((angles->pitch || angles->yaw || angles->roll)) {
		// expand for rotation

		max = RadiusFromBounds(&mins, &maxs);
		for (i = 0; i < 3; i++) {
			mins.data[i] = -max;
			maxs.data[i] =  max;
		}
	}
	if (outmins) VectorCopy(&mins, outmins);
	if (outmaxs) VectorCopy(&maxs, outmaxs);
	if (origin) VectorClear(origin);
}

/*
==================
BotImport_GetMemory
==================
*/
static void *BotImport_GetMemory(int size) {
	void *ptr;

	ptr = svi.Z_TagMalloc( size, TAG_BOTLIB );
	return ptr;
}

/*
==================
BotImport_FreeMemory
==================
*/
static void BotImport_FreeMemory(void *ptr) {
	svi.Z_Free(ptr);
}

/*
=================
BotImport_HunkAlloc
=================
*/
static void *BotImport_HunkAlloc( int size ) {
	if( svi.Hunk_CheckMark() ) {
		svi.Error( ERR_DROP, "SV_Bot_HunkAlloc: Alloc with marks already set" );
	}
	return svi.Hunk_Alloc( size, h_high );
}

/*
==================
BotImport_DebugPolygonCreate
==================
*/
int BotImport_DebugPolygonCreate(int color, int numPoints, vector3 *points) {
	bot_debugpoly_t *poly;
	int i;

	if (!debugpolygons)
		return 0;

	for (i = 1; i < bot_maxdebugpolys; i++) 	{
		if (!debugpolygons[i].inuse)
			break;
	}
	if (i >= bot_maxdebugpolys)
		return 0;
	poly = &debugpolygons[i];
	poly->inuse = qtrue;
	poly->color = color;
	poly->numPoints = numPoints;
	memcpy(poly->points, points, numPoints * sizeof(vector3));
	//
	return i;
}

/*
==================
BotImport_DebugPolygonShow
==================
*/
static void BotImport_DebugPolygonShow(int id, int color, int numPoints, vector3 *points) {
	bot_debugpoly_t *poly;

	if (!debugpolygons) return;
	poly = &debugpolygons[id];
	poly->inuse = qtrue;
	poly->color = color;
	poly->numPoints = numPoints;
	memcpy(poly->points, points, numPoints * sizeof(vector3));
}

/*
==================
BotImport_DebugPolygonDelete
==================
*/
void BotImport_DebugPolygonDelete(int id)
{
	if (!debugpolygons) return;
	debugpolygons[id].inuse = qfalse;
}

/*
==================
BotImport_DebugLineCreate
==================
*/
static int BotImport_DebugLineCreate(void) {
	vector3 points[1];
	return BotImport_DebugPolygonCreate(0, 0, points);
}

/*
==================
BotImport_DebugLineDelete
==================
*/
static void BotImport_DebugLineDelete(int line) {
	BotImport_DebugPolygonDelete(line);
}

/*
==================
BotImport_DebugLineShow
==================
*/
static void BotImport_DebugLineShow(int line, vector3 *start, vector3 *end, int color) {
	vector3 points[4], dir, cross, up = {0, 0, 1};
	float dot;

	VectorCopy(start, &points[0]);
	VectorCopy(start, &points[1]);
	//points[1][2] -= 2;
	VectorCopy(end, &points[2]);
	//points[2][2] -= 2;
	VectorCopy(end, &points[3]);


	VectorSubtract(end, start, &dir);
	VectorNormalize(&dir);
	dot = DotProduct(&dir, &up);
	if (dot > 0.99 || dot < -0.99) VectorSet(&cross, 1, 0, 0);
	else CrossProduct(&dir, &up, &cross);

	VectorNormalize(&cross);

	VectorMA(&points[0],  2, &cross, &points[0]);
	VectorMA(&points[1], -2, &cross, &points[1]);
	VectorMA(&points[2], -2, &cross, &points[2]);
	VectorMA(&points[3],  2, &cross, &points[3]);

	BotImport_DebugPolygonShow(line, color, 4, points);
}

/*
==================
SV_BotClientCommand
==================
*/
static void BotClientCommand( int client, char *command ) {
	SV_ExecuteClientCommand( &svs.clients[client], command, qtrue );
}

/*
==================
SV_BotFrame
==================
*/
void SV_BotFrame( int time ) {
	if (!bot_enable) return;
	//NOTE: maybe the game is already shutdown
	if (!ge.BotAIStartFrame)
		return;
	ge.BotAIStartFrame( time );
}

/*
===============
SV_BotLibSetup
===============
*/
int SV_BotLibSetup( void ) {
	if (!bot_enable) {
		return 0;
	}

	if ( !botlib_export ) {
		Com_Printf( S_COLOR_RED "Error: SV_BotLibSetup without SV_BotInitBotLib\n" );
		return -1;
	}

	return botlib_export->BotLibSetup();
}

/*
===============
SV_ShutdownBotLib

Called when either the entire server is being killed, or
it is changing to a different game directory.
===============
*/
int SV_BotLibShutdown( void ) {

	if ( !botlib_export ) {
		return -1;
	}

	return botlib_export->BotLibShutdown();
}

/*
==================
SV_BotInitCvars
==================
*/
void SV_BotInitCvars(void) {

	svi.Cvar_Get("bot_enable",				"1",	CVAR_NONE,	NULL );						//enable the bot
	svi.Cvar_Get("bot_developer",			"0",	CVAR_CHEAT,	NULL );			//bot developer mode
	svi.Cvar_Get("bot_debug",				"0",	CVAR_CHEAT,	NULL );				//enable bot debugging
	svi.Cvar_Get("bot_maxdebugpolys",		"2",	CVAR_NONE,	NULL );				//maximum number of debug polys
	svi.Cvar_Get("bot_groundonly",			"1",	CVAR_NONE,	NULL );					//only show ground faces of areas
	svi.Cvar_Get("bot_reachability",		"0",	CVAR_NONE,	NULL );				//show all reachabilities to other areas
	svi.Cvar_Get("bot_visualizejumppads",	"0",	CVAR_CHEAT,	NULL );	//show jumppads
	svi.Cvar_Get("bot_forceclustering",		"0",	CVAR_NONE,	NULL );			//force cluster calculations
	svi.Cvar_Get("bot_forcereachability",	"0",	CVAR_NONE,	NULL );			//force reachability calculations
	svi.Cvar_Get("bot_forcewrite",			"0",	CVAR_NONE,	NULL );					//force writing aas file
	svi.Cvar_Get("bot_aasoptimize",			"0",	CVAR_NONE,	NULL );				//no aas file optimisation
	svi.Cvar_Get("bot_saveroutingcache",	"0",	CVAR_NONE,	NULL );			//save routing cache
	svi.Cvar_Get("bot_thinktime",			"100",	CVAR_CHEAT,	NULL );		//msec the bots thinks
	svi.Cvar_Get("bot_reloadcharacters",	"0",	CVAR_NONE,	NULL );			//reload the bot characters each time
	svi.Cvar_Get("bot_testichat",			"0",	CVAR_NONE,	NULL );					//test ichats
	svi.Cvar_Get("bot_testrchat",			"0",	CVAR_NONE,	NULL );					//test rchats
	svi.Cvar_Get("bot_testsolid",			"0",	CVAR_CHEAT,	NULL );			//test for solid areas
	svi.Cvar_Get("bot_testclusters",		"0",	CVAR_CHEAT,	NULL );		//test the AAS clusters
	svi.Cvar_Get("bot_fastchat",			"0",	CVAR_NONE,	NULL );					//fast chatting bots
	svi.Cvar_Get("bot_nochat",				"0",	CVAR_NONE,	NULL );						//disable chats
	svi.Cvar_Get("bot_pause",				"0",	CVAR_CHEAT,	NULL );				//pause the bots thinking
	svi.Cvar_Get("bot_report",				"0",	CVAR_CHEAT,	NULL );			//get a full report in ctf
	svi.Cvar_Get("bot_grapple",				"0",	CVAR_NONE,	NULL );					//enable grapple
	svi.Cvar_Get("bot_rocketjump",			"1",	CVAR_NONE,	NULL );					//enable rocket jumping
	svi.Cvar_Get("bot_challenge",			"0",	CVAR_NONE,	NULL );					//challenging bot
	svi.Cvar_Get("bot_minplayers",			"0",	CVAR_NONE,	NULL );					//minimum players in a team or the game
	svi.Cvar_Get("bot_interbreedchar",		"",		CVAR_CHEAT,	NULL );		//bot character used for interbreeding
	svi.Cvar_Get("bot_interbreedbots",		"10",	CVAR_CHEAT,	NULL );	//number of bots used for interbreeding
	svi.Cvar_Get("bot_interbreedcycle",		"20",	CVAR_CHEAT,	NULL );	//bot interbreeding cycle
	svi.Cvar_Get("bot_interbreedwrite",		"",		CVAR_CHEAT,	NULL );	//write interbreeded bots to this file
}

/*
==================
SV_BotInitBotLib
==================
*/
void SV_BotInitBotLib(void) {
	botlib_import_t	botlib_import;
	GetBotLibAPI_t GetBotLibAPI;
	char dllName[MAX_OSPATH] = "botlib_"ARCH_STRING DLL_EXT;

	if( !(botlibLib = svi.Sys_LoadDll( dllName, qfalse )) )
	{
		Com_Printf( "failed:\n\"%s\"\n", Sys_LibraryError() );
		svi.Error( ERR_FATAL, "Failed to load botlib" );
	}

	GetBotLibAPI = (GetBotLibAPI_t)Sys_LoadFunction( botlibLib, "GetBotLibAPI" );
	if ( !GetBotLibAPI )
		svi.Error(ERR_FATAL, "Can't load symbol GetBotLibAPI: '%s'",  Sys_LibraryError());

	if (debugpolygons) svi.Z_Free(debugpolygons);
	bot_maxdebugpolys = svi.Cvar_VariableIntegerValue("bot_maxdebugpolys");
	debugpolygons = svi.Z_Malloc(sizeof(bot_debugpoly_t) * bot_maxdebugpolys);

	botlib_import.Print = BotImport_Print;
	botlib_import.Error = svi.Error;
	botlib_import.Trace = BotImport_Trace;
	botlib_import.EntityTrace = BotImport_EntityTrace;
	botlib_import.PointContents = BotImport_PointContents;
	botlib_import.inPVS = BotImport_inPVS;
	botlib_import.BSPEntityData = BotImport_BSPEntityData;
	botlib_import.BSPModelMinsMaxsOrigin = BotImport_BSPModelMinsMaxsOrigin;
	botlib_import.BotClientCommand = BotClientCommand;

	//memory management
	botlib_import.GetMemory = BotImport_GetMemory;
	botlib_import.FreeMemory = BotImport_FreeMemory;
	botlib_import.AvailableMemory = svi.Z_AvailableMemory;
	botlib_import.HunkAlloc = BotImport_HunkAlloc;

	// file system access
	botlib_import.FS_FOpenFile = svi.FS_FOpenFileByMode;
	botlib_import.FS_Read = svi.FS_Read2;
	botlib_import.FS_Write = svi.FS_Write;
	botlib_import.FS_FCloseFile = svi.FS_FCloseFile;
	botlib_import.FS_Seek = svi.FS_Seek;

	//debug lines
	botlib_import.DebugLineCreate = BotImport_DebugLineCreate;
	botlib_import.DebugLineDelete = BotImport_DebugLineDelete;
	botlib_import.DebugLineShow = BotImport_DebugLineShow;

	//debug polygons
	botlib_import.DebugPolygonCreate = BotImport_DebugPolygonCreate;
	botlib_import.DebugPolygonDelete = BotImport_DebugPolygonDelete;

	botlib_export = (botlib_export_t *)GetBotLibAPI( BOTLIB_API_VERSION, &botlib_import );
	assert(botlib_export); 	// somehow we end up with a zero import.
}


//
//  * * * BOT AI CODE IS BELOW THIS POINT * * *
//

/*
==================
SV_BotGetConsoleMessage
==================
*/
int SV_BotGetConsoleMessage( int client, char *buf, int size )
{
	client_t	*cl;
	int			index;

	cl = &svs.clients[client];
	cl->lastPacketTime = svs.time;

	if ( cl->reliableAcknowledge == cl->reliableSequence ) {
		return qfalse;
	}

	cl->reliableAcknowledge++;
	index = cl->reliableAcknowledge & ( MAX_RELIABLE_COMMANDS - 1 );

	if ( !cl->reliableCommands[index][0] ) {
		return qfalse;
	}

	Q_strncpyz( buf, cl->reliableCommands[index], size );
	return qtrue;
}

#if 0
/*
==================
EntityInPVS
==================
*/
int EntityInPVS( int client, int entityNum ) {
	client_t			*cl;
	clientSnapshot_t	*frame;
	int					i;

	cl = &svs.clients[client];
	frame = &cl->frames[cl->netchan.outgoingSequence & PACKET_MASK];
	for ( i = 0; i < frame->num_entities; i++ )	{
		if ( svs.snapshotEntities[(frame->first_entity + i) % svs.numSnapshotEntities].number == entityNum ) {
			return qtrue;
		}
	}
	return qfalse;
}
#endif

/*
==================
SV_BotGetSnapshotEntity
==================
*/
int SV_BotGetSnapshotEntity( int client, int sequence ) {
	client_t			*cl;
	clientSnapshot_t	*frame;

	cl = &svs.clients[client];
	frame = &cl->frames[cl->netchan.outgoingSequence & PACKET_MASK];
	if (sequence < 0 || sequence >= frame->num_entities) {
		return -1;
	}
	return svs.snapshotEntities[(frame->first_entity + sequence) % svs.numSnapshotEntities].number;
}
