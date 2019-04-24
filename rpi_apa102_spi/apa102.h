/**********************************************
 *
 *	APA102 LED
 *
 *
 * ************************************************/
#ifndef _APA102_H
#define _APA102_H

struct RGBA;

struct RGBA
{
	union
	{
		uint8_t raw[4];

		struct
		{
			//A/W
			union
			{
				uint8_t a;
				uint8_t alpha;
				uint8_t bri;
				uint8_t brightness;
			};
			//B
			union
			{
				uint8_t b;
				uint8_t blue;
			};
			//G
			union
			{
				uint8_t g;
				uint8_t green;
			};
			//R
			union
			{
				uint8_t r;
				uint8_t red;
			};
		};
	};

	inline RGBA() __attribute__((always_inline))
	{
	}

	inline RGBA(uint8_t ir, uint8_t ig, uint8_t ib, uint8_t ibri) __attribute__((always_inline))
	{
		r = ir;
		g = ig;
		b = ib;
		bri = 0B11100000 | (0B00011111 & ibri);

		return *this;
	}

	//WWRRGGBB
	inline GRBW& operator= (const uint32_t colorcode) __attribute__((always_inline))
	{
		r = (colorcode >> 16) & 0xFF;
		g = (colorcode >> 8)  & 0xFF;
		b = colorcode & 0xFF;
		bri = 0B11100000 | (0B00011111 & (colorcode >> 24));

		return *this;
	}
}


#endif
