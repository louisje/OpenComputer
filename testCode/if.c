int main() {
  int a=1, b=0, c=0;
  if (a==b) c=1;
  if (a!=b) c=2;
  if (a>b) c=3;
  if (a<b) c=4;
  if (a>=b) c=5;
  if (a<=b) c=6;
  if (a && b) c=7;
  if (a || b) c=8;
  if ((a && b)||c) c=9;
  return c;
}