EventScript_UseSurf::
	checkfieldmove FIELD_MOVE_SURF
	goto_if_eq VAR_RESULT, PARTY_SIZE, EventScript_EndUseSurf
	bufferpartymonnick STR_VAR_1, VAR_RESULT
	setfieldeffectargument 0, VAR_RESULT
	lockall
	msgbox gText_WantToUseSurf, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EventScript_ReleaseUseSurf
	goto_if_eq VAR_0x8005, 2, EventScript_UseSurfToolOnly
	msgbox gText_PlayerUsedSurf, MSGBOX_DEFAULT
	goto EventScript_UseSurfStartEffect
EventScript_UseSurfToolOnly:
	bufferitemname STR_VAR_2, ITEM_SURFBOARD
	msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
EventScript_UseSurfStartEffect:
	call_if_eq VAR_0x8005, 2, EventScript_FieldToolSkipMonCutIn
	dofieldeffect FLDEFF_USE_SURF
EventScript_ReleaseUseSurf::
	releaseall
EventScript_EndUseSurf::
	end

EventScript_CurrentTooFast::
	lockall
	checkfieldmove FIELD_MOVE_SURF
	goto_if_eq VAR_RESULT, FALSE, EventScript_CurrentTooFastEndMsg
	playse SE_SELECT
	msgbox Text_CurrentTooFast
EventScript_CurrentTooFastEndMsg::
	releaseall
	end
