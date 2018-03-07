#pragma once

struct BodyEnumInfo_t
{
	int body;
	int nummodels;
};

extern int CalcBody( BodyEnumInfo_t *info, int count );