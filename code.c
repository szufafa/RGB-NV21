
void RgbToNv21(const unsigned char* rgb, int width, int height, unsigned char* nv21) {
    int ySize = width * height;
    unsigned char* yBase = nv21;
    unsigned char* uvBase = nv21 + ySize;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            int rgbIdx = (j * width + i) * 3;
            int r = rgb[rgbIdx];
            int g = rgb[rgbIdx + 1];
            int b = rgb[rgbIdx + 2];

            // 计算Y分量
            yBase[j*width + i] = (77*r + 150*g + 29*b) >> 8; // 0.299≈77/256
            
            // 每2x2块计算UV分量
            if ((i % 2 == 0) && (j % 2 == 0)) {
                int avgR = (r + rgb[rgbIdx+3] + rgb[rgbIdx + width*3] + rgb[rgbIdx + width*3 +3]) >> 2;
                int avgG = (g + rgb[rgbIdx+4] + rgb[rgbIdx + width*3 +1] + rgb[rgbIdx + width*3 +4]) >> 2;
                int avgB = (b + rgb[rgbIdx+5] + rgb[rgbIdx + width*3 +2] + rgb[rgbIdx + width*3 +5]) >> 2;

                int u = ((-38*avgR - 74*avgG + 112*avgB) >> 8) + 128; // -0.148≈-38/256
                int v = ((112*avgR - 94*avgG - 18*avgB) >> 8) + 128;  // 0.439≈112/256

                int uvIdx = (j/2)*(width/2) + (i/2);
                uvBase[uvIdx*2] = v;   // NV21存储顺序为VU交替
                uvBase[uvIdx*2 +1] = u;
            }
        }
    }
}
// YUV420 NV21转RGB（优化实现）
void Nv21ToRgb(const unsigned char* yuv, int width, int height, unsigned char* rgb) {
    const unsigned char* y = yuv;
    const unsigned char* uv = yuv + width * height;
    
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            int y_val = y[j * width + i];
            int uv_idx = ((j >> 1) * (width >> 1) + (i >> 1)) << 1;
            int u_val = uv[uv_idx + 1]; // NV21格式：VU交错存储
            int v_val = uv[uv_idx];

            // 转换为RGB（BT.601标准）
            y_val = y_val < 16 ? 16 : (y_val > 235 ? 235 : y_val);
            int c = y_val - 16;
            int d = u_val - 128;
            int e = v_val - 128;

            int r = (298 * c + 409 * e + 128) >> 8;
            int g = (298 * c - 100 * d - 208 * e + 128) >> 8;
            int b = (298 * c + 516 * d + 128) >> 8;

            // 钳制到0-255范围
            rgb[(j * width + i) * 3 + 0] = (unsigned char)(r < 0 ? 0 : (r > 255 ? 255 : r));
            rgb[(j * width + i) * 3 + 1] = (unsigned char)(g < 0 ? 0 : (g > 255 ? 255 : g));
            rgb[(j * width + i) * 3 + 2] = (unsigned char)(b < 0 ? 0 : (b > 255 ? 255 : b));
        }
    }
}
