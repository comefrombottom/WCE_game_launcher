#pragma once
constexpr RectF layoutRect(const RectF& rect, double width, double height)
{
	return RectF(Arg::center = rect.center(), width, height);
}


constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, double width, double height)
{
	return RectF(Arg::leftCenter(rect.leftX() + left.value(), rect.centerY()), width, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, SizeF size)
{
	return layoutRect(rect, left, size.x, size.y);
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, double width, double height)
{
	return RectF(Arg::rightCenter(rect.rightX() - right.value(), rect.centerY()), width, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, SizeF size)
{
	return layoutRect(rect, right, size.x, size.y);
}

constexpr RectF layoutRect(const RectF& rect, Arg::top_<double> top, double width, double height)
{
	return RectF(Arg::topCenter(rect.centerX(), rect.topY() + top.value()), width, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::top_<double> top, SizeF size)
{
	return layoutRect(rect, top, size.x, size.y);
}

constexpr RectF layoutRect(const RectF& rect, Arg::bottom_<double> bottom, double width, double height)
{
	return RectF(Arg::bottomCenter(rect.centerX(), rect.bottomY() - bottom.value()), width, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::bottom_<double> bottom, SizeF size)
{
	return layoutRect(rect, bottom, size.x, size.y);
}


constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::top_<double> top, double width, double height)
{
	return RectF(Arg::topLeft(rect.leftX() + left.value(), rect.topY() + top.value()), width, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::top_<double> top, SizeF size)
{
	return layoutRect(rect, left, top, size.x, size.y);
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::bottom_<double> bottom, double width, double height)
{
	return RectF(Arg::bottomLeft(rect.leftX() + left.value(), rect.bottomY() - bottom.value()), width, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::bottom_<double> bottom, SizeF size)
{
	return layoutRect(rect, left, bottom, size.x, size.y);
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, Arg::top_<double> top, double width, double height)
{
	return RectF(Arg::topRight(rect.rightX() - right.value(), rect.topY() + top.value()), width, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, Arg::top_<double> top, SizeF size)
{
	return layoutRect(rect, right, top, size.x, size.y);
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, Arg::bottom_<double> bottom, double width, double height)
{
	return RectF(Arg::bottomRight(rect.rightX() - right.value(), rect.bottomY() - bottom.value()), width, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, Arg::bottom_<double> bottom, SizeF size)
{
	return layoutRect(rect, right, bottom, size.x, size.y);
}


constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::right_<double> right, double height)
{
	return RectF(Arg::leftCenter(rect.leftX() + left.value(), rect.centerY()), rect.w - left.value() - right.value(), height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::top_<double> top, Arg::bottom_<double> bottom, double width)
{
	return RectF(Arg::topCenter(rect.centerX(), rect.topY() + top.value()), width, rect.h - top.value() - bottom.value());
}


constexpr RectF  layoutRect(const RectF& rect, Arg::left_<double> left, Arg::right_<double> right, Arg::top_<double> top, double height)
{
	return RectF(Arg::topLeft(rect.leftX() + left.value(), rect.topY() + top.value()), rect.w - left.value() - right.value(), height);
}

constexpr RectF  layoutRect(const RectF& rect, Arg::left_<double> left, Arg::right_<double> right, Arg::bottom_<double> bottom, double height)
{
	return RectF(Arg::bottomLeft(rect.leftX() + left.value(), rect.bottomY() - bottom.value()), rect.w - left.value() - right.value(), height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::top_<double> top, Arg::bottom_<double> bottom, double width)
{
	return RectF(Arg::topLeft(rect.leftX() + left.value(), rect.topY() + top.value()), width, rect.h - top.value() - bottom.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, Arg::top_<double> top, Arg::bottom_<double> bottom, double width)
{
	return RectF(Arg::topRight(rect.rightX() - right.value(), rect.topY() + top.value()), width, rect.h - top.value() - bottom.value());
}

#if false

// width, height を指定しない場合フルに伸長

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left)
{
	return RectF(rect.x + left.value(), rect.y, rect.w - left.value(), rect.h);
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right)
{
	return RectF(rect.x, rect.y, rect.w - right.value(), rect.h);
}

constexpr RectF layoutRect(const RectF& rect, Arg::top_<double> top)
{
	return RectF(rect.x, rect.y + top.value(), rect.w, rect.h - top.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::bottom_<double> bottom)
{
	return RectF(rect.x, rect.y, rect.w, rect.h - bottom.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, double width)
{
	return RectF(Arg::leftCenter(rect.leftX() + left.value(), rect.centerY()), width, rect.h);
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, double width)
{
	return RectF(Arg::rightCenter(rect.rightX() - right.value(), rect.centerY()), width, rect.h);
}

constexpr RectF layoutRect(const RectF& rect, Arg::top_<double> top, double height)
{
	return RectF(Arg::topCenter(rect.centerX(), rect.topY() + top.value()), rect.w, height);
}

constexpr RectF layoutRect(const RectF& rect, Arg::bottom_<double> bottom, double height)
{
	return RectF(Arg::bottomCenter(rect.centerX(), rect.bottomY() - bottom.value()), rect.w, height);
}


constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::top_<double> top)
{
	return RectF(rect.x + left.value(), rect.y + top.value(), rect.w - left.value(), rect.h - top.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::bottom_<double> bottom)
{
	return RectF(rect.x + left.value(), rect.y, rect.w - left.value(), rect.h - bottom.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, Arg::top_<double> top)
{
	return RectF(rect.x, rect.y + top.value(), rect.w - right.value(), rect.h - top.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, Arg::bottom_<double> bottom)
{
	return RectF(rect.x, rect.y, rect.w - right.value(), rect.h - bottom.value());
}


constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::right_<double> right)
{
	return RectF(rect.x + left.value(), rect.y, rect.w - left.value() - right.value(), rect.h);
}

constexpr RectF layoutRect(const RectF& rect, Arg::top_<double> top, Arg::bottom_<double> bottom)
{
	return RectF(rect.x, rect.y + top.value(), rect.w, rect.h - top.value() - bottom.value());
}


constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::right_<double> right, Arg::top_<double> top)
{
	return RectF(Arg::topLeft(rect.leftX() + left.value(), rect.topY() + top.value()), rect.w - left.value() - right.value(), rect.h - top.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::right_<double> right, Arg::bottom_<double> bottom)
{
	return RectF(Arg::bottomLeft(rect.leftX() + left.value(), rect.bottomY() - bottom.value()), rect.w - left.value() - right.value(), rect.h - bottom.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::left_<double> left, Arg::top_<double> top, Arg::bottom_<double> bottom)
{
	return RectF(Arg::topLeft(rect.leftX() + left.value(), rect.topY() + top.value()), rect.w - left.value(), rect.h - top.value() - bottom.value());
}

constexpr RectF layoutRect(const RectF& rect, Arg::right_<double> right, Arg::top_<double> top, Arg::bottom_<double> bottom)
{
	return RectF(Arg::topRight(rect.rightX() - right.value(), rect.topY() + top.value()), rect.w - right.value(), rect.h - top.value() - bottom.value());
}

#endif


constexpr RectF layoutRect(RectF rect, Arg::left_<double> left, Arg::right_<double> right, Arg::top_<double> top, Arg::bottom_<double> bottom)
{
	return RectF(rect.leftX() + left.value(), rect.topY() + top.value(), rect.w - left.value() - right.value(), rect.h - top.value() - bottom.value());
}


// separate Rect (長さ)

constexpr std::pair<RectF, RectF> separateRect(const RectF& rect, Arg::left_<double> left)
{
	return { RectF{rect.x, rect.y, left.value(), rect.h}, RectF{rect.x + left.value(), rect.y, rect.w - left.value(), rect.h} };
}

constexpr std::pair<RectF, RectF> separateRect(const RectF& rect, Arg::right_<double> right)
{
	return { RectF{rect.x + rect.w - right.value(), rect.y, right.value(), rect.h}, RectF{rect.x, rect.y, rect.w - right.value(), rect.h} };
}

constexpr std::pair<RectF, RectF> separateRect(const RectF& rect, Arg::top_<double> top)
{
	return { RectF{rect.x, rect.y, rect.w, top.value()}, RectF{rect.x, rect.y + top.value(), rect.w, rect.h - top.value()} };
}

constexpr std::pair<RectF, RectF> separateRect(const RectF& rect, Arg::bottom_<double> bottom)
{
	return { RectF{rect.x, rect.y + rect.h - bottom.value(), rect.w, bottom.value()}, RectF{rect.x, rect.y, rect.w, rect.h - bottom.value()} };
}

// separate Rect (割合)

constexpr std::pair<RectF, RectF> separateRectRatio(const RectF& rect, Arg::left_<double> left)
{
	const double w = rect.w * left.value();
	return separateRect(rect, Arg::left = w);
}

constexpr std::pair<RectF, RectF> separateRectRatio(const RectF& rect, Arg::right_<double> right)
{
	const double w = rect.w * right.value();
	return separateRect(rect, Arg::right = w);
}

constexpr std::pair<RectF, RectF> separateRectRatio(const RectF& rect, Arg::top_<double> top)
{
	const double h = rect.h * top.value();
	return separateRect(rect, Arg::top = h);
}

constexpr std::pair<RectF, RectF> separateRectRatio(const RectF& rect, Arg::bottom_<double> bottom)
{
	const double h = rect.h * bottom.value();
	return separateRect(rect, Arg::bottom = h);
}
