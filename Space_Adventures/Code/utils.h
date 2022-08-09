#pragma once
#include <math.h>
#include<random>

std::mt19937_64 rnd(std::random_device{}());
//std::mt19937 rnd(42); // random

namespace Utils{
	typedef char s8;
	typedef unsigned char u8;
	typedef short s16;
	typedef unsigned short u16;
	typedef int s32;
	typedef unsigned int u32;
	typedef long long s64;
	typedef unsigned long long u64;

	#define global_variable static
	#define internal static

	/* return number beetween [_min, _val]*/
	template <typename num_type>
	internal num_type my_clamp(num_type _min, num_type _val, num_type _max) {
		return (_val < _min) ? _min : ((_val > _max) ? _max : _val);
	}

	/* return true if val [_left, _right] else false*/
	template <typename num_type>
	internal bool is_between(num_type _left, num_type _val, num_type _right) {
		return _left <= _val && _val <= _right;
	}

	// random number on a segment [left, right]
	internal int rnoas(int left, int right) { 
		return rnd() % (right - left + 1) + left;
	}

	/*
	percentage of truth
	percent [1, 100]
	*/
	internal bool chance(int percent) {
		return rnoas(0, 100) <= percent;
	}


	//random object on a segment
	template <typename object_type>
	internal object_type roas(const std::vector<object_type>& A) {
		return A[rnoas(0, A.size() - 1)];
	}

	internal void simulate_move(float& p, float& dp, float ddp, float dt) {
		ddp -= dp * 15.f;
		p = p + dp * dt + ddp * dt * dt * .5f;
		dp = dp + ddp * dt;
	}

	std::string num_to_str(int num) {
		std::string str;
		if (num == 0) {
			str = "0";
		}
		else {
			bool is_negative = false;
			if (num < 0) {
				is_negative = true;
				num *= -1;
			}
			while (num) {
				str += num % 10 + '0';
				num /= 10;
			}
			if (is_negative) {
				str += '-';
			}
			std::reverse(str.begin(), str.end());
		}
		return str;
	}
}
using namespace Utils;