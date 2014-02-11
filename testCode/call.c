void main() {
	int total = sum(10);
}

int sum(int n) {
    int s = 0, i;
    for (i=1; i<=n; i++)
      s = s + i;
	return s;
}
