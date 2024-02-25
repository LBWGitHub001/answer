template <class T>
T max(T num1,T num2){
    if(num1 >= num2)
        return num1;
    if(num2 > num1)
        return num2;
    return 0;
}

template <class T>
T min(T num1,T num2){
    if(num1 <= num2)
        return num1;
    if(num2 < num1)
        return num2;
    return 0;
}