#pragma once
#include "game/backend/SavedVariables.hpp"

namespace VV2
{
	extern void DrawSavedVariableEdit(const SavedVariableBase& var, void* value);
	extern void DrawSavedVariablePreview(const SavedVariableBase& var, void* value);
	extern void DrawSavedVariable(SavedVariableBase& var);
}