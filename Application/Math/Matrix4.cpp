#include "Matrix4.h"
#include "Vector3.h"
#include <cmath> //sin cos

Matrix4 Matrix4Identity() {
	static const Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

Matrix4 Matrix4Scale(const Vector3& s) {
	Matrix4 result{
		s.x,0.0f,0.0f,0.0f,
		0.0f, s.y,0.0f,0.0f,
		0.0f,0.0f, s.z,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

Matrix4 Matrix4Rotation(const Vector3& r) {
	Matrix4 result = Matrix4Identity();
	result *= Matrix4RotationZ(r.z);
	result *= Matrix4RotationX(r.x);
	result *= Matrix4RotationY(r.y);

	return result;
}

Matrix4 Matrix4RotationX(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f, cos, sin,0.0f,
		0.0f,-sin, cos,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

Matrix4 Matrix4RotationY(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		cos,0.0f,-sin,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		sin,0.0f, cos,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

Matrix4 Matrix4RotationZ(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	Matrix4 result{
		cos, sin,0.0f,0.0f,
		-sin, cos,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	return result;
}

Matrix4 Matrix4WorldTransform(const Vector3& scale, const Vector3& rotation, const Vector3& position)
{
	Matrix4 matWorld = Matrix4Identity();

	Matrix4 matScale = Matrix4Scale(scale);

	Matrix4 matRot = Matrix4Rotation(rotation);

	Matrix4 matTrans = Matrix4Translation(position);

	//合成
	matWorld *= matScale *= matRot *= matTrans;

	return matWorld;
}

Matrix4 Matrix4Translation(const Vector3& t) {
	Matrix4 result{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		t.x, t.y, t.z,1.0f,
	};

	return result;
}

Matrix4 Matrix4Transposed(const Matrix4& m) {
	Matrix4 result{
		m.m[0][0],m.m[1][0],m.m[2][0],m.m[3][0],
		m.m[0][1],m.m[1][1],m.m[2][1],m.m[3][1],
		m.m[0][2],m.m[1][2],m.m[2][2],m.m[3][2],
		m.m[0][3],m.m[1][3],m.m[2][3],m.m[3][3],
	};

	return result;
}


Matrix4 Matrix4Inverse(const Matrix4& m) {
	//Matrix4の行数 列数
	const int LineNum = 4;

	//逆行列を求める二次元配列
	double mat[LineNum][LineNum] = {};

	//逆行列保存用二次元配列
	double inv[LineNum][LineNum] = {};

	//掃き出し法を行うための行列
	double sweep[LineNum][LineNum * 2] = {};

	int i, j, focus = 0; //行、列、注目する列

	//一時保存する値
	double keep = 0;

	//許容誤差
	const double MAX_ERR = 1e-10f;

	//最終出力
	Matrix4 result = {};

	//セット
	for (i = 0; i < LineNum; i++) {
		for (j = 0; j < LineNum; j++) {
			mat[i][j] = static_cast<double>(m.m[i][j]);
		}
	}

	//sweepの左半分と右半分に、それぞれ初期設定
	for (i = 0; i < LineNum; i++) {
		for (j = 0; j < LineNum; j++) {
			//左半分には逆行列を求めたい行列をセット
			sweep[i][j] = mat[i][j];

			//右半分には単位行列をセット
			sweep[i][LineNum + j] = (i == j) ? 1 : 0;
		}
	}

	//全ての列の対角成分に対する繰り返し
	for (focus = 0; focus < LineNum; focus++) {
		//ここから0除算対策

		//最大の絶対値を注目対角成分の絶対値と仮定
		double max = fabs(sweep[focus][focus]);
		int max_i = focus;

		//focus列目が最大の絶対値となる行を探す
		for (i = focus + 1; i < LineNum; i++) {
			if (fabs(sweep[i][focus]) > max) {
				max = fabs(sweep[i][focus]);
				max_i = i;
			}
		}
		//逆行列が求められないことが発覚したら、強制終了
		if (fabs(sweep[max_i][focus]) <= MAX_ERR) {
			return m;
		}

		if (focus != max_i) {
			for (j = 0; j < LineNum * 2; j++) {
				float tmp = static_cast<float>(sweep[max_i][j]);
				sweep[max_i][j] = sweep[focus][j];
				sweep[focus][j] = tmp;
			}
		}
		//ここまで0除算対策

		//ここから掃き出し法
		//sweep[focus][focus]に掛けると1になる値
		keep = 1 / sweep[focus][focus];

		//focus行目をkeep倍する
		for (j = 0; j < LineNum * 2; j++) {
			//sweep[focus][focus]に掛ける
			sweep[focus][j] *= keep;
		}

		for (i = 0; i < LineNum; i++) {
			if (i == focus) {
				//k行目はそのままスルー
				continue;
			}

			//focus列目を0にするための値
			keep = -sweep[i][focus];

			for (j = 0; j < LineNum * 2; j++) {
				//i行目にfocus行目をbuf倍した行を足す
				sweep[i][j] += sweep[focus][j] * keep;
			}
		}
		//ここまで掃き出し法
	}

	//sweepの右半分がmの逆行列
	for (i = 0; i < LineNum; i++) {
		for (j = 0; j < LineNum; j++) {
			inv[i][j] = sweep[i][LineNum + j];
			result.m[i][j] = static_cast<float>(sweep[i][LineNum + j]);
		}
	}

	return result;
}

Vector3 Vector3CrossMatrix4(const Vector3& v, const Matrix4& m) {
	Vector3 result = {
		v.x * m.m[0][0] +
		v.y * m.m[1][0] +
		v.z * m.m[2][0] +
		0 * m.m[3][0],

		v.x * m.m[0][1] +
		v.y * m.m[1][1] +
		v.z * m.m[2][1] +
		0 * m.m[3][1],

		v.x * m.m[0][2] +
		v.y * m.m[1][2] +
		v.z * m.m[2][2] +
		0 * m.m[3][1]
	};

	return result;
}

Vector3 Coord3dCrossMatrix4(const Vector3& v, const Matrix4& m) {
	Vector3 result = {
		v.x * m.m[0][0] +
		v.y * m.m[1][0] +
		v.z * m.m[2][0] +
		1 * m.m[3][0],

		v.x * m.m[0][1] +
		v.y * m.m[1][1] +
		v.z * m.m[2][1] +
		1 * m.m[3][1],

		v.x * m.m[0][2] +
		v.y * m.m[1][2] +
		v.z * m.m[2][2] +
		1 * m.m[3][2]
	};

	return result;
}

Vector3 Vector3TransformCoord(const Vector3& v, const Matrix4& m) {
	float w =
		v.x * m.m[0][3] +
		v.y * m.m[1][3] +
		v.z * m.m[2][3] +
		(1 * m.m[3][3]);

	Vector3 result = {
		(v.x * m.m[0][0] +
		 v.y * m.m[1][0] +
		 v.z * m.m[2][0] +
		 m.m[3][0]) / w,

		(v.x * m.m[0][1] +
		 v.y * m.m[1][1] +
		 v.z * m.m[2][1] +
		 m.m[3][1]) / w,

		(v.x * m.m[0][2] +
		 v.y * m.m[1][2] +
		 v.z * m.m[2][2] +
		 m.m[3][2]) / w,
	};

	return result;
}

Matrix4& operator*=(Matrix4& m1, const Matrix4& m2) {
	Matrix4 result = { 0 };

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}

	return m1 = result;
}

const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2) {
	Matrix4 result = m1;
	return result *= m2;
}

const Vector3 operator*(const Vector3& v, const Matrix4& m) {
	return Vector3CrossMatrix4(v, m);
}