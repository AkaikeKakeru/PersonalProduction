#include "AdjustmentVariables.h"

void AdjustmentVariables::CreateGroup(const std::string& groupName) {
	//指定名のオブジェクトが無ければ追加
	datas_[groupName];
}

AdjustmentVariables* AdjustmentVariables::GetInstance() {
	static AdjustmentVariables instance;
	return &instance;
}
