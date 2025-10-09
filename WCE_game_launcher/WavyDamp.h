#pragma once

template <class VectorType, class ValueType>
VectorType WavyDamp_impl(
	const VectorType& from,
	const VectorType& to,
	VectorType& velocity,
	const ValueType smoothTime,    // 従来の smoothTime (これから ω=2/smoothTime と定義)
	const ValueType dampingRatio,          // 減衰比 (damping ratio)
	const ValueType deltaTime,
	const Optional<ValueType>& maxSpeed = none
)
{
	// natural frequency ω を smoothTime から定義
	const ValueType w = static_cast<ValueType>(2.0) / smoothTime;

	// 位置誤差
	VectorType X = from - to;

	// maxSpeed 制約（変化量を制限）
	/*if (maxSpeed)
	{
		const ValueType maxChange = (*maxSpeed) * smoothTime * dampingRatio;
		if constexpr (std::is_arithmetic_v<VectorType>)
		{
			X = Clamp(X, -maxChange, maxChange);
		}
		else
		{
			X = X.limitLength(maxChange);
		}
	}*/

	// ここから2次系の解法を用いて1ステップ先を計算

	if (dampingRatio > 1)
	{
		// 過減衰 (Overdamped): ζ > 1
		// 特性方程式の解: r1, r2 = -ζω ± ω√(ζ²-1)
		ValueType alpha = w * std::sqrt(dampingRatio * dampingRatio - 1);
		ValueType r1 = -dampingRatio * w + alpha;
		ValueType r2 = -dampingRatio * w - alpha;

		// 一般解: 
		// x(t) = to + C1 e^(r1 t) + C2 e^(r2 t)
		// v(t) = C1 r1 e^(r1 t) + C2 r2 e^(r2 t)
		// 初期条件: x(0)=from, v(0)=velocity
		// よって
		// from = to + C1 + C2
		// velocity = C1 r1 + C2 r2
		// 連立方程式より:
		// C1 + C2 = X
		// C1 r1 + C2 r2 = v
		// C1 = (v - X*r2)/(r1 - r2)
		// C2 = X - C1

		ValueType exp1 = std::exp(r1 * deltaTime);
		ValueType exp2 = std::exp(r2 * deltaTime);

		VectorType C1 = (velocity - X * r2) / (r1 - r2);
		VectorType C2 = X - C1;

		velocity = C1 * r1 * exp1 + C2 * r2 * exp2;
		return from - X + C1 * exp1 + C2 * exp2;
	}
	else if (dampingRatio < 1)
	{
		// 不足減衰 (Underdamped): ζ < 1
		// 固有振動周波数: wd = ω√(1 - ζ²)
		ValueType wd = w * std::sqrt(1 - dampingRatio * dampingRatio);

		// 解:
		// x(t) = to + e^{−ζω t} [ X cos(wd t) + ((v + ζω X)/wd) sin(wd t) ]
		ValueType expTerm = std::exp(-dampingRatio * w * deltaTime);

		ValueType cos_wd = std::cos(wd * deltaTime);
		ValueType sin_wd = std::sin(wd * deltaTime);

		if (maxSpeed)
		{

			//double expTerm_60 = std::exp(-dampingRatio * w * 1.0 / 60);
			//double cos_wd_60 = std::cos(wd * 1.0 / 60);
			//double sin_wd_60 = std::sin(wd * 1.0 / 60);
			//const ValueType maxChange = (*maxSpeed) * (1 + expTerm_60 * (dampingRatio * w / wd * sin_wd_60 - cos_wd_60)) / (w * w * expTerm_60 / wd * sin_wd_60);
			const ValueType maxChange = (*maxSpeed) * (1 + expTerm * (dampingRatio * w / wd * sin_wd - cos_wd)) / (w * w * expTerm / wd * sin_wd);
			//const ValueType maxChange = (*maxSpeed) * wd / (w * w) * exp(dampingRatio);

			if constexpr (std::is_arithmetic_v<VectorType>)
			{
				X = Clamp(X, -maxChange, maxChange);
			}
			else
			{
				X = X.limitLength(maxChange);
			}

		}

		VectorType tmp = (velocity + dampingRatio * w * X) / wd;


		// v(t) は上式を時間微分して求める:
		// dx/dt = e^{−ζω t} [ -ζω (X cos(wd t) + tmp sin(wd t))
		//    + (-X wd sin(wd t) + tmp wd cos(wd t)) ]
		velocity = expTerm * (-dampingRatio * w * X * cos_wd - dampingRatio * w * tmp * sin_wd - wd * X * sin_wd + wd * tmp * cos_wd);
		return from - X + expTerm * (X * cos_wd + tmp * sin_wd);
	}
	else
	{
		// 臨界減衰 (Critically damped): ζ = 1
		// ζ=1 の場合は元コードに近い形で処理可能
		// 一般解:
		// x(t) = to + (X + (v + ωX)t) e^{−ω t}
		ValueType expTerm = std::exp(-w * deltaTime);

		if (maxSpeed)
		{
			const ValueType maxChange = (*maxSpeed) * (1 - expTerm * (1 - w * deltaTime)) / (w * w * expTerm * deltaTime);
			if constexpr (std::is_arithmetic_v<VectorType>)
			{
				X = Clamp(X, -maxChange, maxChange);
			}
			else
			{
				X = X.limitLength(maxChange);
			}
		}

		VectorType term = (velocity + w * X) * deltaTime;

		// v(t) の計算
		// dx/dt = ...
		// v_new = (v - w*(X+term))*exp(-w*dt)
		velocity = (velocity - w * term) * expTerm;
		return from - X + (X + term) * expTerm;
	}
}

double WavyDamp(double from, double to, double& velocity, double smoothTime, double dampingRatio, double deltaTime = Scene::DeltaTime(), const Optional<double>& maxSpeed = none)
{
	return WavyDamp_impl<double>(from, to, velocity, smoothTime, dampingRatio, deltaTime, maxSpeed);
}

Vec2 WavyDamp(const Vec2& from, const Vec2& to, Vec2& velocity, double smoothTime, double dampingRatio, double deltaTime = Scene::DeltaTime(), const Optional<double>& maxSpeed = none)
{
	return WavyDamp_impl<Vec2, double>(from, to, velocity, smoothTime, dampingRatio, deltaTime, maxSpeed);
}
