#pragma once

//衝突属性
const unsigned short COLLISION_ATTR_LANDSHAPE = 0b1 << 0;//地形
const unsigned short COLLISION_ATTR_PLAYER = 0b1 << 1;//プレイヤー陣営
const unsigned short COLLISION_ATTR_ENEMYS = 0b1 << 2;//敵属性
const unsigned short COLLISION_ATTR_GOAL = 0b1 << 3;//ゴール属性