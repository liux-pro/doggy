class Box
{
   public:
      int length;   // 盒子的长度
      int breadth;  // 盒子的宽度
      int height;   // 盒子的高度
      int getFuck();
};


int Box::getFuck(void){
    return 123;
}


extern "C" int call_c(void) // wrapper function
{
    Box box;
    return box.getFuck();
}
