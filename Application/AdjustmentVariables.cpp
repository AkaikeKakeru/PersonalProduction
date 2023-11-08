#include "AdjustmentVariables.h"

AdjustmentVariables* AdjustmentVariables::GetInstance() {
	static AdjustmentVariables instance;
	return &instance;
}
