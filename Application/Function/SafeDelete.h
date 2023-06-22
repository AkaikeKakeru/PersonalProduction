#pragma once

template <class T>
void SafeDelete(T* &p) {
	delete p; 
	p = nullptr;
}