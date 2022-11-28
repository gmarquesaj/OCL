float map_from_to(float x, float a, float b, float c, float d) {
  return (x - a) / (b - a) * (d - c) + c;
}
int XY2POS(int xx, int yy, int w, int h) {
  /* if((x>w) || (x<0) || (y>h) || (y<0))
  {
    return 0;
  } */
  int x = xx < 0 ? 0 : xx > w ? w : xx;
  int y = yy < 0 ? 0 : yy > h ? h : yy;
  return (y * w * 4) + (x * 4);
}
void kernel mandelbrotImage2D(__write_only image2d_t entrada, sampler_t amostra,
                              int maxinte, float zoom, float x_pos,
                              float y_pos) {
  int x = get_global_id(0);
  int y = get_global_id(1);
  int w = get_global_size(0);
  // int h = get_global_size(1);

  float b = map_from_to(x, 0.0, w, (x_pos - zoom), (x_pos + zoom));
  float a = map_from_to(y, 0.0, w, (y_pos - zoom), (y_pos + zoom));
  float ca = a;
  float cb = b;
  int g = 0;
  for (int i = 0; i < maxinte; i++) {

    if (sqrt(a * a + b * b) >= 2.0) {
      break;
    }
    float aa = a * a - b * b;

    float bb = 2.0f * a * b;
    a = (aa + ca);
    b = (bb + cb);
    g += 1;
  }
  float val = map_from_to(g, 0.0f, maxinte, 0.0f, 1.0f);
  barrier(CLK_GLOBAL_MEM_FENCE);
  float r = val;

  int2 coords;
  coords.x = x;
  coords.y = y;
  float gg, bb, h, s, v;
  h = 1.0f - (val * 1.9f);
  s = 0.6f;
  v = 1.0f;

  int i = floor(h * 6);
  float f = h * 6 - i;
  float q = v * (1.0f - f * s);
  float p = v * (1 - s);
  float t = v * (1 - (1 - f) * s);
  switch (i % 6) {

  case 0:
    r = v, gg = t, bb = p;
    break;
  case 1:
    r = q, gg = v, bb = p;
    break;
  case 2:
    r = p, gg = v, bb = t;
    break;
  case 3:
    r = p, gg = q, bb = v;
    break;
  case 4:
    r = t, gg = p, bb = v;
    break;
  case 5:
    r = v, gg = p, bb = q;
    break;
  }
  r = r > 1.0f ? 1.0 : r < 0.0f ? 0.0f : r;
  bb = bb > 1.0f ? 1.0 : bb < 0.0f ? 0.0f : bb;
  gg = gg > 1.0f ? 1.0 : gg < 0.0f ? 0.0f : gg;

  if (g == maxinte) {
    r = 0.0f;
    gg = 0.0f;
    bb = 0.0f;
  }
  float4 cor = {r, gg, bb, 1.0f};
  write_imagef(entrada, coords, cor);
}

void kernel mandelbrotVectorFloat(__global float *entrada, int maxinte,
                                  float zoom, float x_pos, float y_pos) {
  const int x = get_global_id(0);
  const int y = get_global_id(1);
  const int w = get_global_size(0);
  const int h = get_global_size(1);

  const int pos = XY2POS(x, y, w, h);
  float b = map_from_to(x, 0.0, w, (x_pos - zoom), (x_pos + zoom));
  float a = map_from_to(y, 0.0, w, (y_pos - zoom), (y_pos + zoom));
  float ca = a;
  float cb = b;
  int g = 0;
  for (int i = 0; i < maxinte; i++) {

    if (sqrt(a * a + b * b) >= 2.0f) {
      break;
    }
    float aa = a * a - b * b;

    float bb = 2.0f * a * b;
    a = (aa + ca);
    b = (bb + cb);
    g += 1;
  }
  float suave = g - log2(log2(a * a + b * b));
  float val = map_from_to(suave, 0.0f, maxinte, 0.0f, 1.0f);
  // printf("x = %d, y = %d , val = %d\n",x,y,val);
  // float val = map_from_to(g, 0.0f, maxinte, 0.0f, 1.0f);
  entrada[pos] = val;
  entrada[pos + 1] = val;

  entrada[pos + 2] = val;
  entrada[pos + 3] = val;
  barrier(CLK_LOCAL_MEM_FENCE);
  barrier(CLK_GLOBAL_MEM_FENCE);

  val = val < 0.0f ? 0.0f : val > 1.0f ? 1.0f : val;
  float r = val;

  int2 coords;
  coords.x = x;
  coords.y = y;
  float gg, bb, hue, s, v;
  hue = 1.0f - (val * 1.9f);
  s = 0.6f;
  v = 1.0f;

  int i = floor(hue * 6);
  float f = hue * 6 - i;
  float q = v * (1.0f - f * s);
  float p = v * (1 - s);
  float t = v * (1 - (1 - f) * s);
  switch (i % 6) {

  case 0:
    r = v, gg = t, bb = p;
    break;
  case 1:
    r = q, gg = v, bb = p;
    break;
  case 2:
    r = p, gg = v, bb = t;
    break;
  case 3:
    r = p, gg = q, bb = v;
    break;
  case 4:
    r = t, gg = p, bb = v;
    break;
  case 5:
    r = v, gg = p, bb = q;
    break;
  }
  r = r > 1.0f ? 1.0 : r < 0.0f ? 0.0f : r;
  bb = bb > 1.0f ? 1.0 : bb < 0.0f ? 0.0f : bb;
  gg = gg > 1.0f ? 1.0 : gg < 0.0f ? 0.0f : gg;

  if (g == maxinte) {
    r = 0.0f;
    gg = 0.0f;
    bb = 0.0f;
  }
  float4 cor = {r, gg, bb, 1.0f};
  // write_imagef(entrada, coords,cor);
  entrada[pos] = cor.x;
  entrada[pos + 1] = cor.y;
  entrada[pos + 2] = cor.z;
  entrada[pos + 3] = 1.0f;
}
typedef struct {
  int x, y;
  float r, g, b;
} ponto;
int dis(int x1, int x2, int y1, int y2)
{
	int dx = (x1 - x2);
	int dy = (y1 - y2);
	// return abs(dx)  + abs(dy) ;

	return dx * dx + dy * dy;
};
void kernel voronoy(__global float *entrada, __global ponto *pontos,const int q) {
  const int x = get_global_id(0);
  const int y = get_global_id(1);
  const int w = get_global_size(0);
  const int h = get_global_size(1);

  const int pos = XY2POS(x, y, w, h);

  int indexPrx = 0;
  for (int i = 1; i < q; i++) {

    ponto p = pontos[i];
    ponto p2 = pontos[indexPrx];
    if (dis(p.x, x, p.y, y) < dis(p2.x, x, p2.y, y)) {
      indexPrx = i;
    }
  }
  ponto p = pontos[indexPrx];

//  ponto p = pontos[pos % 9];
  float4 cor = {p.r, p.g, p.b, 1.0f};
  // float4 cor = {1.0f, 0.5f,1.0f, 1.0f};

  entrada[pos] = cor.x;
  entrada[pos + 1] = cor.y;
  entrada[pos + 2] = cor.z;
  entrada[pos + 3] = 1.0f;
}