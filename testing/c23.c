
float add(int x, int y) {
    return x + y;
}

int mod(int x, int y) {
    while (x >= y) {
        x = x - y;

        if (x < y) {
            return x;
        }
    }
}
