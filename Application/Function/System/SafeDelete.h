/*�|�C���^�̍폜��null����������܂Ƃ߂�*/

#pragma once

template <class T>
//�Z�[�t�f���[�g
void SafeDelete(T* &p) {
	delete p; 
	p = nullptr;
}