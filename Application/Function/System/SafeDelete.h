/*ポインタの削除とnull初期化を一まとめに*/

#pragma once

template <class T>
//セーフデリート
void SafeDelete(T* &p) {
	delete p; 
	p = nullptr;
}