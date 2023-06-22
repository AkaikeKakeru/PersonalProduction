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
	//for文用繰り返し数
	const int LineNum = 4;

	//行
	int i = 0;
	//列
	int j = 0;
	//注目対角成分が存在する列
	int focus = 0;

	//最終出力
	Matrix4 result = {};
	//掃き出し法用行列
	float sweep[LineNum][LineNum * 2] = {};

	//sweepの左半分と右半分に、それぞれ初期設定
	for (i = 0; i < LineNum; i++) {
		for (j = 0; j < LineNum; j++) {
			//左半分には逆行列を求めたい行列をセット
			sweep[i][j] = m.m[i][j];

			//右半分には単位行列をセット
			if (i == j) {
				sweep[i][LineNum + j] = 1;
			}
			else {
				sweep[i][LineNum + j] = 0;
			}
		}
	}

	//掃き出し法で逆行列を組み立てていく
	for (focus = 0; focus < LineNum; focus++) {
		/*最大の絶対値を注目対角成分の絶対値と仮定する*/
		//最大の絶対値
		float max = fabs(sweep[focus][focus]);
		//最大の絶対値が含まれる行
		int max_i = focus;

		//focus列目が最大値の絶対値となる行を探す
		for (i = focus + 1; i < LineNum; i++) {
			if (fabs(sweep[i][focus]) > max) {
				max = fabs(sweep[i][focus]);
				max_i = i;
			}
		}

		//許容する誤差の範囲
		const double MAX_ERR = 1e-10;

		//逆行列が求められないことが発覚したら、強制終了
		if (static_cast<double>(fabs(sweep[max_i][focus])) <= MAX_ERR) {
			return m;
		}

		//focus行目とmax_i行目を入れ替える
		if (focus != max_i) {
			for (j = 0; j < LineNum * 2; j++) {
				float tmp = sweep[max_i][j];
				sweep[max_i][j] = sweep[focus][j];
				sweep[focus][j] = tmp;
			}
		}


		//sweep[focus][focus]に1掛けると1になる値
		float normalize = 1 / sweep[focus][focus];

		//focus行目をnormalize倍する
		for (j = 0; j < LineNum * 2; j++) {
			//sweep[focus][j]を1に
			sweep[focus][j] *= normalize;
		}

		/*forcus行目以外の行のfocus列目を0にする*/
		for (i = 0; i < LineNum; i++) {
			//focus行目はそのまま
			if (i == focus) {
				continue;
			}

			//k行目に掛ける値を求める
			float zeroization = -sweep[i][focus];

			for (j = 0; j < LineNum * 2; j++) {
				//sweep[i][focus]を0にする
				sweep[i][j] += sweep[focus][j] * zeroization;
			}
		}
	}

	/*sweepの右半分が、求めていた逆行列*/
	for (i = 0; i < LineNum; i++) {
		for (j = 0; j < LineNum; j++) {
			result.m[i][j] = sweep[i][LineNum + j];
		}
	}

	return result;
}

Vector3 Vector3Transform(const Vector3& v, const Matrix4& m) {
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + (1 * m.m[3][3]);

	Vector3 result = {
	(v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
	(v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
	(v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w,
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
	return Vector3Transform(v, m);
}