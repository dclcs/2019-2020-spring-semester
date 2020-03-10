public static int numZero(int[] arr) {
    // Effects: if arr is null throw NullPointerException
    // else return the number of occurences of 0 in arr
    int count = 0;
    for (int i = 1; i < arr.length; i++) {
        if (arr[i] == 0) {
            count++;
        }
    }
    return count;
}