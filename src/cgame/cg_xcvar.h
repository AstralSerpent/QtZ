
#ifdef XCVAR_PROTO
	#define XCVAR_DEF( name, defVal, update, flags, desc ) extern cvar_t *name;
#endif

#ifdef XCVAR_DECL
	#define XCVAR_DEF( name, defVal, update, flags, desc ) cvar_t *name = NULL;
#endif

#ifdef XCVAR_LIST
	#define XCVAR_DEF( name, defVal, update, flags, desc ) { & name , #name , defVal , update , flags , desc },
#endif

//			name							default value			update callback			flags						description
XCVAR_DEF( bg_synchronousClients,			"0",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( cg_autoswitch,					"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_brightModels,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_centertime,					"2",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_chatboxCompletion,			"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_chatboxLineCount,				"5",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_chatboxMsgTime,				"5",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairColour,				"255 255 255",			CG_XHairColorUpdate,	CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairCooldownSpread,		"16",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairCooldownTics,		"16",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairCooldownWeapons,		"27",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairDisableZoomed,		"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairFeedbackColour,		"255 0 0",				CG_XHairColorUpdate,	CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairFeedbackTime,		"200",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairHealth,				"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_crosshairSize,				"24",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_currentSelectedPlayer,		"-1",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( cg_currentSelectedPlayerName,	"",						NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_damageViewKick,				"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_debugEvents,					"0",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( cg_debugHUD,						"0",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( cg_debugScoreboard,				"0",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( cg_deferPlayers,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_delagHitscan,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_draw2D,						"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_draw3dIcons,					"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawAmmoWarning,				"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawRewards,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawAttacker,					"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawCrosshair,				"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawCrosshairNames,			"1",					NULL,					CVAR_ARCHIVE,				NULL )
//XCVAR_DEF( cg_drawFlagCarrier,				"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawFPS,						"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawGun,						"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawIcons,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawPing,						"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawSnapshot,					"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawStatus,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_drawTeamOverlay,				"1",					CG_TeamOverlayUpdate,	CVAR_ARCHIVE|CVAR_USERINFO,	NULL )
XCVAR_DEF( cg_drawTimer,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_errorDecay,					"100",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_flagScale,					"0.5",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_forceAllyModel,				"none",					CG_ForceModelUpdate,	CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_forceAllyModelColor,			"127 127 127",			CG_ForceColorUpdate,	CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_forceEnemyModel,				"none",					CG_ForceModelUpdate,	CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_forceEnemyModelColor,			"0 255 0",				CG_ForceColorUpdate,	CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_footsteps,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_fov,							"90",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_fovAspectAdjust,				"1",					NULL,					CVAR_ARCHIVE,				"Automatically scale the field of view for non 4:3 aspect ratios" )
XCVAR_DEF( cg_fovViewmodel,					"0",					NULL,					CVAR_ARCHIVE,				"Field of view for first-person view models. 0 = inherit from cg_fov" )
XCVAR_DEF( cg_gibs,							"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_gunAlign,						"0.0 0.0 0.0",			CG_GunAlignUpdate,		CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_gunBob,						"0.005 0.01 0.005",		CG_ViewVarsUpdate,		CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_gunBobEnable,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_gunIdleDrift,					"0.01 0.01 0.01 0.004",	CG_ViewVarsUpdate,		CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_gunIdleDriftEnable,			"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_gunZoomAlign,					"-2.0 4.0 2.35",		CG_GunAlignUpdate,		CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_hudFiles,						"ui/hud.txt",			NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_instantDuck,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
//XCVAR_DEF( cg_itemPickupTime,				"3500",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_lagometer,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_lagometerPos,					"592 432",				NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_marks,						"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_model,						DEFAULT_MODEL,			NULL,					CVAR_USERINFO|CVAR_ARCHIVE,	NULL )
XCVAR_DEF( cg_noTaunt,						"0",					NULL,					CVAR_ARCHIVE,				NULL )
//XCVAR_DEF( cg_obituary,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
//XCVAR_DEF( cg_obituaryTime,				"5000",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_predictItems,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_runpitch,						"0.002",				NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_runroll,						"0.005",				NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_scorePlums,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_shadows,						"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_simpleItems,					"1",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_smoothClients,				"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_swingSpeed,					"0.3",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_teamChatsOnly,				"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_teamChatTime,					"3000",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_timescaleFadeEnd,				"1",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( cg_timescaleFadeSpeed,			"0",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( cg_trueLightning,				"1.0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_viewBob,						"0.002 0.002 0.005 1",	CG_ViewVarsUpdate,		CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_viewBobEnable,				"0",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_zoomFov,						"60",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cg_zoomTime,						"0.55",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( cl_paused,						"0",					NULL,					CVAR_ROM,					NULL )
XCVAR_DEF( com_cameraMode,					"0",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( com_frametime,					"8",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( timescale,						"1",					NULL,					CVAR_NONE,					NULL )
XCVAR_DEF( ui_bigFont,						"0.4",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( ui_smallFont,					"0.25",					NULL,					CVAR_ARCHIVE,				NULL )
XCVAR_DEF( pm_acceleration,					"16.0",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_bunnyHopDebounce,				"125",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_bunnyHopEnable,				"1",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_doubleJumpDebounce,			"300",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_doubleJumpEnable,				"1",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_doubleJumpPush,				"100",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_frametime,					"20",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_friction,						"8.0",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_gravity,						"1100",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_jumpVelocity,					"300.0",				NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_overbounce,					"0",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_rampJumpEnable,				"1",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_float,						"1",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_speed,						"320",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_wallJumpEnable,				"1",					NULL,					CVAR_SYSTEMINFO,			NULL )
XCVAR_DEF( pm_wallJumpDebounce,				"900",					NULL,					CVAR_SYSTEMINFO,			NULL )

#undef XCVAR_DEF
