  float map_from_to(float x, float a, float b, float c, float d) {
  return (x - a) / (b - a) * (d - c) + c;
}
  void kernel mandelbrot(__write_only image2d_t entrada, sampler_t 
  amostra,int maxinte,float zoom,float x_pos,float y_pos){
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
       float r = val;
   
       int2  coords;
       coords.x=x;
       coords.y=y;
       float gg, bb, h, s, v;
  h = 1.0f-(val*1.9f);
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
  r = r>1.0f?1.0:r<0.0f?0.0f:r;
  bb = bb>1.0f?1.0:bb<0.0f?0.0f:bb;
  gg = gg>1.0f?1.0:gg<0.0f?0.0f:gg;
  
  if (g == maxinte) {
    r = 0.0f;
    gg = 0.0f;
    bb = 0.0f;
  }
       float4 cor ={r,gg,bb,1.0f};
  write_imagef(entrada, coords,cor);
  }   