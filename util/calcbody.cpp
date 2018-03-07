
#include <calcbody.h>

int CalcBody( BodyEnumInfo_t *info, int count )
{
	int		body = 0;
	int		base;
	bool	valid;

	if ( count <= 0 )
		return 0;

	do
	{
		valid = true;

		for ( int i = 0; i < count; i++ )
		{
			if ( i )
				base *= info[i - 1].nummodels;
			else
				base = 1;

			if ( body / base % info[i].nummodels != info[i].body )
			{
				valid = false;
				break;
			}
		}

		if ( valid )
			return body;

		body++;
	}
	while ( body <= 255 );

	return 0;
}