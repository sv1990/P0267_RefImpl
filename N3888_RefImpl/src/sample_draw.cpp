#include "sample_draw.h"
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <Windows.h>

using namespace std;
using namespace std::experimental::drawing;

vector<vector<int>> init_sort_steps(int count, unsigned long mtSeed = 1009UL) {
	vector<vector<int>> result;
	result.push_back([count, mtSeed]() {
		vector<int> init;
		for (int i = 0; i < count; ++i) {
			init.push_back(i);
		}
		mt19937 rng(mtSeed);
		shuffle(begin(init), end(init), rng);
		return init;
	}());
	bool notSorted = true;
	while (notSorted) {
		vector<int> curr(result.back());
		const auto size = curr.size();
		notSorted = false;
		for (auto i = 0U; i < size - 1; ++i) {
			if (curr[i] > curr[i + 1]) {
				notSorted = true;
				auto temp = curr[i + 1];
				curr[i + 1] = curr[i];
				curr[i] = temp;
			}
		}
		if (notSorted) {
			result.push_back(curr);
		}
	}
	return result;
}

wostream& operator<<(wostream& os, const point& pt) {
	os << L"(" << pt.x << L"," << pt.y << L")";
	return os;
}

void sample_draw::operator()(surface& rs, double elapsedTimeInMilliseconds) {
	static double timer = 0.0;
	const double power = 3.0, lerpTime = 1250.0, phaseTime = lerpTime + 500.0, pi = 3.1415926535897932;
	const double normalizedTime = min(fmod(timer, phaseTime) / lerpTime, 1.0);
	const double adjustment = (normalizedTime < 0.5) ? pow(normalizedTime * 2.0, power) / 2.0 :
		((1.0 - pow(1.0 - ((normalizedTime - 0.5) * 2.0), power)) * 0.5) + 0.5;
	const int elementCount = 12;
	const static auto vec = init_sort_steps(elementCount);
	const int phaseCount = static_cast<int>(vec.size()), x = min(static_cast<int>(timer / phaseTime), phaseCount - 1);
	auto cornflowerBluePattern = solid_color_pattern_builder(rgba_color::cornflower_blue).get_pattern();
	rs.set_pattern(cornflowerBluePattern);
	rs.paint(); // Paint background.
	double left, top, right, bottom;
	point lt, rb;
	rs.clip_extents(lt, rb);
	left = lt.x;
	top = lt.y;
	right = rb.x;
	bottom = rb.y;
	const double radius = trunc(min((right - left) * 0.8 / elementCount, (bottom - top) + 120.0) / 2.0);
	const double beginX = trunc((right - left) * 0.1), y = trunc((bottom - top) * 0.5);
	path_builder pb;
	pb.move_to({ beginX, 50.0 });
	rs.set_path(pb.get_path());
	auto whitePattern = solid_color_pattern_builder(rgba_color::white).get_pattern();
	rs.set_pattern(whitePattern);
	rs.select_font_face("Segoe UI", font_slant::normal, font_weight::normal);
	rs.set_font_size(40.0);
	rs.show_text(string("Phase ").append(to_string(x + 1)).c_str());

	for (int i = 0; i < elementCount; ++i) {
		path_builder pb;
		const auto currVal = vec[x][i];
		if (x < phaseCount - 1) {
			const auto i2 = find(begin(vec[x + 1]), end(vec[x + 1]), currVal) - begin(vec[x + 1]);
			const auto x1r = radius * i * 2.0 + radius + beginX, x2r = radius * i2 * 2.0 + radius + beginX;
			const auto yr = y - ((i2 == i ? 0.0 : (radius * 4.0 * (normalizedTime < 0.5 ? normalizedTime : 1.0 - normalizedTime)))
				* (i % 2 == 1 ? 1.0 : -1.0));
			const auto center = point{ trunc((x2r - x1r) * adjustment + x1r), trunc(yr) };
			pb.set_transform_matrix(matrix::init_scale({ 1.0, 1.5 }) * matrix::init_rotate(pi / 4.0) * matrix::init_translate({ 0.0, 50.0 }));
			pb.set_origin(center);
			pb.arc_negative(center, radius - 3.0, pi / 2.0, 3.0 * pi / 2.0);
		}
		else {
			const point center{ radius * i * 2.0 + radius + beginX, y };
			pb.set_transform_matrix(matrix::init_scale({ 1.0, 1.5 }) * matrix::init_rotate(pi / 4.0));
			pb.set_origin(center);
			pb.arc_negative(center, radius - 3.0, pi / 2.0, 3.0 * pi / 2.0);
		}
		rs.set_path(pb.get_path());
		double greyColor = 1.0 - (currVal / (elementCount - 1.0));
		auto greyPattern = solid_color_pattern_builder({ greyColor, greyColor, greyColor, 1.0 }).get_pattern();
		rs.set_pattern(greyPattern);
		rs.fill();
	}

	timer = (timer > phaseTime * (phaseCount + 2)) ? 0.0 : timer + elapsedTimeInMilliseconds;
}
