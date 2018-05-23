#pragma once

enum class PageReplacementPolicy
{
	FIRST_IN_FIRST_OUT,
	LEAST_RECENTLY_USED,
	LEAST_FREQUENTLY_USED,
	MOST_RECENTLY_USED
};
