#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "math.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "osd_truetype.h"
#include "bitmap.h"

#define MY_UTF8_INVALID 0xFFFD //internal invalid utf8 rune
#define MY_UTF8_SIZE 4 //describes the number of bytes a glyph consists of*/
#define MY_LEN(a) (sizeof(a)/sizeof(a)[0])
#define MY_BETWEEN(x, a, b) ((a) <= (x) && (x) < (b))
static const uint8_t MY_UTF8BYTE[MY_UTF8_SIZE + 1] = { 0x80, 0, 0xC0, 0xE0, 0xF0 };
static const uint8_t MY_UTF8MASK[MY_UTF8_SIZE + 1] = { 0xC0, 0x80, 0xE0, 0xF0, 0xF8 };
static const uint32_t MY_UTF8MIN[MY_UTF8_SIZE + 1] = { 0, 0, 0x80, 0x800, 0x10000 };
static const uint32_t MY_UTF8MAX[MY_UTF8_SIZE + 1] = { 0x10FFFF, 0x7F, 0x7FF, 0xFFFF, 0x10FFFF };

stbtt_fontinfo fontinfo;
static int fontInitCompleted = 0;

static int utf8Validate(uint32_t* u, int i)
{
    if (!u)
    {
        return 0;
    }
    if (!MY_BETWEEN(*u, MY_UTF8MIN[i], MY_UTF8MAX[i]) || MY_BETWEEN(*u, 0xD800, 0xDFFF))
    {
        *u = MY_UTF8_INVALID;
    }
    for (i = 1; *u > MY_UTF8MAX[i]; ++i);
    return i;
}

static uint32_t utf8DecodeByte(char c, int* i)
{
    if (!i)
    {
        return 0;
    }
    for (*i = 0; *i < (int)MY_LEN(MY_UTF8MASK); ++(*i))
    {
        if (((uint8_t)c & MY_UTF8MASK[*i]) == MY_UTF8BYTE[*i])
        {
            return (uint8_t)(c & ~MY_UTF8MASK[*i]);
        }
    }
    return 0;
}

static int utf8Decode(char* c, uint32_t* u, int clen)
{
    int i, j, len, type = 0;
    uint32_t udecoded;

    if (!c || !u || !clen)
    {
        return 0;
    }
    *u = MY_UTF8_INVALID;
    udecoded = utf8DecodeByte(c[0], &len);
    if (!MY_BETWEEN(len, 1, MY_UTF8_SIZE))
    {
        return 1;
    }
    for (i = 1, j = 1; i < clen && j < len; ++i, ++j)
    {
        udecoded = (udecoded << 6) | utf8DecodeByte(c[i], &type);
        if (type != 0)
        {
            return j;
        }
    }
    if (j < len)
    {
        return 0;
    }
    *u = udecoded;
    utf8Validate(u, len);
    return len;
}

static int fontinfoInit(void)
{
    int iRet = 0;
    long int size = 0;
    unsigned char* fontBuffer = NULL;

    FILE *fontFile = fopen("/tmp/default.ttf", "rb");
    if (fontFile == NULL)
    {
        printf("Can not open font file!");
        return -1;
    }

    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    fontBuffer = (unsigned char*)calloc(size, sizeof(unsigned char));
    if(NULL == fontBuffer)
    {
        printf("fontBuffer calloc failed");
        return -1;
    }
    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    iRet = stbtt_InitFont(&fontinfo, fontBuffer, 0);
    if (0 == iRet)
    {
        printf("stb init font failed");
    }
    //free(fontBuffer);
    fontInitCompleted = 1;

    return iRet;
}

static int alignTo(int n, int align)
{
    return (n + align - 1) & ~(align - 1);
}

unsigned char * textToBitmap(unsigned char *text,float scale,int bitmap_w,int bitmap_h, unsigned char* inputBitmap)
{
	unsigned char* bitmap = NULL;

    //创建位图
	if(NULL == inputBitmap) {
		bitmap = (unsigned char*)calloc(bitmap_w * bitmap_h, sizeof(unsigned char));
		if(NULL == bitmap){
			printf("calloc failed!\n");
			return NULL;
		}
	} else {
		bitmap = inputBitmap;
	}

    int ascent = 0;
    int descent = 0;
    int lineGap = 0;
    //取得竖直方向上的度量
    stbtt_GetFontVMetrics(&fontinfo, &ascent, &descent, &lineGap);
    //根据缩放调整字高
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);
  //  printf("ascent:%d,descent:%d",ascent,descent);
    //位图的x
    int x = 0;
    //循环加载text中每个字符
    int text_len = strlen((char *)text);
    int curr_len = 0;
    int next_len = 0;
    uint32_t word = 0;
    uint32_t next_word = 0;
    ////////////////////////////////////////
    // 获取水平方向上的度量
    // advanceWidth：字宽；
    // leftSideBearing：左侧位置；
    int advanceWidth = 0;
    int leftSideBearing = 0;
    int c_x1 = 0;
    int c_y1 = 0;
    int c_x2 = 0;
    int c_y2 = 0;
    //计算位图的y (不同字符的高度不同）
    int y = 0;
    //渲染字符
    int byteOffset = 0;
    //调整字距
    int kern = 0;
    next_len = utf8Decode((char *)(text + curr_len), &word, text_len - curr_len);

    //printf("text_len:%u",text_len);
    while (next_len)
    {
        curr_len += next_len;
        // 获取水平方向上的度量
        advanceWidth = leftSideBearing = 0;
        stbtt_GetCodepointHMetrics(&fontinfo, word, &advanceWidth, &leftSideBearing);
        //printf("advanceWidth:%d,leftSideBearing:%d\n",advanceWidth,leftSideBearing);
        //获取字符的边框（边界）
        c_x1 = c_y1 = c_x2 = c_y2 = 0;
        stbtt_GetCodepointBitmapBox(&fontinfo, word, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
        //计算位图的y (不同字符的高度不同）
        y = ascent + c_y1;
        //渲染字符
        byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmap_w);
        stbtt_MakeCodepointBitmap(&fontinfo, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, bitmap_w, scale, scale, word);
        //调整x
        x += roundf(advanceWidth * scale);

        next_len = utf8Decode((char *)(text + curr_len), &next_word, text_len - curr_len);
        if (next_len > 0)
        {
            //调整字距
            kern = stbtt_GetCodepointKernAdvance(&fontinfo, word, next_word);
            x += roundf(kern * scale);
            word = next_word;
        }
    }

    return bitmap;
}

int getBitmapTrueWidth(unsigned char *text, float pixels)
{
    //int text_num = getUtf8Length((char *)text);
    //计算字体缩放
    float scale = stbtt_ScaleForPixelHeight(&fontinfo, pixels); //scale = pixels / (ascent - descent)
    int x = 0;
    int text_len = strlen((char *)text);
    int curr_len = 0;
    int next_len = 0;
    uint32_t word = 0;
    uint32_t next_word = 0;
    int advanceWidth = 0;
    int leftSideBearing = 0;
    //int c_x1 = 0;
    //int c_y1 = 0;
    //int c_x2 = 0;
    //int c_y2 = 0;
    int kern = 0;
    next_len = utf8Decode((char *)(text + curr_len), &word, text_len - curr_len);
    while (next_len)
    {
        curr_len += next_len;
        advanceWidth = leftSideBearing = 0;
        stbtt_GetCodepointHMetrics(&fontinfo, word, &advanceWidth, &leftSideBearing);
        x += roundf(advanceWidth * scale);
        next_len = utf8Decode((char *)(text + curr_len), &next_word, text_len - curr_len);
        if (next_len > 0)
        {
            kern = stbtt_GetCodepointKernAdvance(&fontinfo, word, next_word);
            x += roundf(kern * scale);
            word = next_word;
        }
    }
    return x;
}

unsigned char * getArgbDataAfterHandleStr(unsigned char *text,int *width,int *height, float pixels ,int bitmap_h,  unsigned char* inputBitmap,bool b_change)
{
	unsigned char *argbBuf = NULL;
	unsigned char* bitmap = NULL;

	if(0 == fontInitCompleted){
		fontinfoInit();
	}

    //计算字体缩放
    float scale = stbtt_ScaleForPixelHeight(&fontinfo, pixels); //scale = pixels / (ascent - descent)

    int bitmap_w_true = getBitmapTrueWidth(text,pixels);
    int bitmap_w = alignTo(bitmap_w_true,8);

    *width = bitmap_w;
    *height = bitmap_h;

	if(NULL == inputBitmap) {
	    argbBuf = (unsigned char*)malloc(bitmap_w*bitmap_h*2);
		if(NULL == argbBuf)
		{
			printf("malloc error");
			free(bitmap);
			return NULL;
		}

		bitmap = textToBitmap(text,scale,bitmap_w,bitmap_h, NULL);
	    bitmapToArgb1555(argbBuf, bitmap_w*bitmap_h*2, bitmap, bitmap_w,bitmap_h,b_change);
		free(bitmap);
	} else {
	    bitmap = textToBitmap(text,scale,bitmap_w,bitmap_h, inputBitmap);
		argbBuf = inputBitmap + bitmap_w * bitmap_h;
		bitmapToArgb1555(argbBuf, bitmap_w*bitmap_h*2, bitmap, bitmap_w,bitmap_h,b_change);
	}

	return (argbBuf);
}

unsigned char * getArgbDataAfterHandleStr_2bit(unsigned char *text,int *width,int *height, float pixels ,int bitmap_h,  unsigned char* inputBitmap)
{
	unsigned char *argbBuf = NULL;
	unsigned char* bitmap = NULL;

	if(0 == fontInitCompleted){
		fontinfoInit();
	}

    //计算字体缩放
    float scale = stbtt_ScaleForPixelHeight(&fontinfo, pixels); //scale = pixels / (ascent - descent)

    int bitmap_w_true = getBitmapTrueWidth(text,pixels);
	
    int bitmap_w = alignTo(bitmap_w_true,64);
	
    *width = bitmap_w;
    *height = pixels;
	
	if(NULL == inputBitmap) {
	    argbBuf = (unsigned char*)malloc((bitmap_w*bitmap_h)/4);
		if(NULL == argbBuf)
		{
			printf("malloc error");
			free(bitmap);
			return NULL;
		}

		bitmap = textToBitmap(text,scale,bitmap_w,bitmap_h, NULL);
	    bitmapToArgb2bit(argbBuf, (bitmap_w*bitmap_h)/4, bitmap, bitmap_w,bitmap_h);
		free(bitmap);
	} else {
	    bitmap = textToBitmap(text,scale,bitmap_w,bitmap_h, inputBitmap);
		argbBuf = inputBitmap + bitmap_w * bitmap_h;
		bitmapToArgb2bit(argbBuf, (bitmap_w*bitmap_h)/4, bitmap, bitmap_w,bitmap_h);
	}

	return (argbBuf);
}

