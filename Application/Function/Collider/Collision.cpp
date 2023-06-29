#include "Collision.h"
#include "MyMath.h"

const float Collision::EPSILON_ = 1.0e-5f;

bool Collision::CheckSphere2Sphere(const Sphere& sphereA,
	const Sphere& sphereB, Vector3* inter) {

	float distance =
		(sphereB.center_.x - sphereA.center_.x) * (sphereB.center_.x - sphereA.center_.x)
		+ (sphereB.center_.y - sphereA.center_.y) * (sphereB.center_.y - sphereA.center_.y)
		+ (sphereB.center_.z - sphereA.center_.z) * (sphereB.center_.z - sphereA.center_.z);

	float radian =
		(sphereA.radius_ + sphereB.radius_) * (sphereA.radius_ + sphereB.radius_);

	//疑似交点を計算
	if (inter) {

	}

	if (distance <= radian) {
		return true;
	}

	return false;
}

bool Collision::CheckSphere2Plane(const Sphere& sphere,
	const Plane& plane, Vector3* inter) {
	//座標系の原点から球の中心座標への距離
	float distV = Vector3Dot(sphere.center_, plane.normal_);

	//平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = distV - plane.distance_;

	//距離の絶対値が半径より大きければ当たっていない
	if (AbsoluteValue(dist) > sphere.radius_) return false;

	//疑似交点を計算
	if (inter) {
		//平面上の最近接点を、疑似交点とする
		*inter = -dist * plane.normal_ + sphere.center_;
	}

	return true;
}

void Collision::ClosestPtPoint2Triangle(const Vector3& point, const Triangle& triangle, Vector3* closest) {
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	Vector3 p0_p1 = triangle.p1_ - triangle.p0_;
	Vector3 p0_p2 = triangle.p2_ - triangle.p0_;
	Vector3 p0_pt = point - triangle.p0_;

	float d1 = Vector3Dot(p0_p1, p0_pt);
	float d2 = Vector3Dot(p0_p2, p0_pt);

	if (d1 <= 0.0f && d2 <= 0.0f) {
		// p0が最近傍
		*closest = triangle.p0_;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	Vector3 p1_pt = point - triangle.p1_;

	float d3 = Vector3Dot(p0_p1, p1_pt);
	float d4 = Vector3Dot(p0_p2, p1_pt);

	if (d3 >= 0.0f && d4 <= d3) {
		// p1が最近傍
		*closest = triangle.p1_;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		*closest = triangle.p0_ + v * p0_p1;
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	Vector3 p2_pt = point - triangle.p2_;

	float d5 = Vector3Dot(p0_p1, p2_pt);
	float d6 = Vector3Dot(p0_p2, p2_pt);
	if (d6 >= 0.0f && d5 <= d6) {
		*closest = triangle.p2_;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		*closest = triangle.p0_ + w * p0_p2;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1_ + w * (triangle.p2_ - triangle.p1_);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0_ + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter) {
	Vector3 p;

	//球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center_, triangle, &p);

	//点pと球の中心の差分ベクトル
	Vector3 v = p - sphere.center_;

	//距離の二乗を求める
	//(同じベクトル同士の内積は三平方の定理のルート内部の式と一致する)
	float vDot = Vector3Dot(v, v);

	//球と三角形の距離が半径以下なら当たっていない
	if (vDot > sphere.radius_ * sphere.radius_) return false;

	//疑似交点を計算
	if (inter) {
		//三角形上の最近接点pを疑似交点とする
		*inter = p;
	}

	return true;
}

bool Collision::CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance, Vector3* inter) {

	//面法線とレイの方向ベクトルの内積
	float d1 = Vector3Dot(plane.normal_, ray.dir_);

	//裏面には当たらない
	if (d1 > -EPSILON_) { return false; }

	//視点と原点の距離(平面の法線方向)
	//面法線とレイの始点座標の内積
	float d2 = Vector3Dot(plane.normal_, ray.start_);

	//始点と平面の距離(平面の法線方向)
	float dist = d2 - plane.distance_;

	//始点と平面の距離(レイ方向)
	float t = dist / -d1;

	//交点が始点より後ろにあるので、当たらない
	if (t < 0.0f)return false;

	//距離を書き込む
	if (distance) { *distance = t; }

	//交点を計算
	if (inter) { *inter = ray.start_ + t * ray.dir_; }

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance, Vector3* inter) {
	//三角形が乗っている平面を算出
	Plane plane;
	Vector3 interPlace;
	plane.normal_ = triangle.normal_;
	plane.distance_ = Vector3Dot(triangle.normal_, triangle.p0_);

	//レイと平面が当たっていなければ、当たっていない
	if (!CheckRay2Plane(ray, plane, distance, &interPlace)) { return false; }

	//レイと平面が当たっていたので、距離と交点が書き込まれた
	//レイと平面の交点が三角形の内側にあるかを判定
	Vector3 m;

	//辺p0_p1について
	Vector3 pt_p0 = triangle.p0_ - interPlace;
	Vector3 p0_p1 = triangle.p1_ - triangle.p0_;
	m = Vector3Cross(pt_p0, p0_p1);
	//辺の外側であれば当たっていないので判定を打ち切る
	if (Vector3Dot(m, triangle.normal_) < -EPSILON_) { return false; }

	//辺p1_p2について
	Vector3 pt_p1 = triangle.p1_ - interPlace;
	Vector3 p1_p2 = triangle.p2_ - triangle.p1_;
	m = Vector3Cross(pt_p1, p1_p2);
	//辺の外側であれば当たっていないので判定を打ち切る
	if (Vector3Dot(m, triangle.normal_) < -EPSILON_) { return false; }

	//辺p2_p0について
	Vector3 pt_p2 = triangle.p2_ - interPlace;
	Vector3 p2_p0 = triangle.p0_ - triangle.p2_;
	m = Vector3Cross(pt_p2, p1_p2);
	//辺の外側であれば当たっていないので判定を打ち切る
	if (Vector3Dot(m, triangle.normal_) < -EPSILON_) { return false; }

	//内側なので当たっている
	if (inter) {
		*inter = interPlace;
	}

	return true;
}

bool Collision::CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance, Vector3* inter) {
	Vector3 m = ray.start_ - sphere.center_;
	float a = Vector3Dot(ray.dir_,ray.dir_);
	float b = Vector3Dot(m, ray.dir_);
	float c = Vector3Dot(m, m) - (sphere.radius_ * sphere.radius_);

	if (a == 0.0f) return false;//レイの長さが0

	//rayの始点がsphereの外側にあり(c > 0)、rayがsphereから離れていく方向を指している場合(b > 0)、当たらない
	if (c > 0.0f && b > 0.0f) { return false; }

	float discr = b * b - c;
	//負の判別式は球が外れていることに一致
	if (discr < 0.0f) { return false; }

	//レイは球と交差している
	//交差する最小の値を計算
	float t = -b - SquareRoot(discr);

	if (t < 0.0f) return false;//衝突していない

	//tが負である場合、レイが球の内側から開始しているので、tをゼロにクランプ
	if (t < 0.0f)t = 0.0f;
	if (distance) { *distance = t; }

	if (inter) { *inter = ray.start_ + t * ray.dir_; }

	return true;
}
