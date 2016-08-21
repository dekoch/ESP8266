
//code from https://github.com/MarquisdeGeek/arduino-fractal/
//changed a little bit

typedef float imaginaryaxe;
typedef uint16_t color;

class Mandelbrot
{ 
  private:
    float fXmin;
    float fXmax;
    float fYmin;
    float fYmax;
    int intMaxIteration;
    const int escape_velocity = 10;

    int intImageWidth;
    int intImageHeight;

    color colorBackground;
    color colorForeground;
 
  public:
    void SetMandelbrotParameters(float xmin, float xmax, float ymin, float ymax, int iteration)
    {
      fXmin = xmin;
      fXmax = xmax;
      fYmin = ymin;
      fYmax = ymax;
      intMaxIteration = iteration;
    }

    void SetImageColors(color background, color foreground)
    {
      colorBackground = background;
      colorForeground = foreground;
    }

    void SetImageSize(int width, int height)
    {
      intImageWidth = width;
      intImageHeight = height;
    }
 
    color ComputeArea(imaginaryaxe cx, imaginaryaxe cy)
    {   
      int iteration = 0;
      imaginaryaxe x1 = 0;
      imaginaryaxe y1 = 0;
      imaginaryaxe x2 = 0;
      imaginaryaxe y2 = 0;
      imaginaryaxe temp = 0;
      
      while(true)
      {
        if (iteration >= intMaxIteration)
        {
          return colorForeground;
        } 
        else if ((x2 + y2) > escape_velocity)
        {
          return colorBackground;
        }
      
        temp = x2 - y2 + cx;
        y1 = (2 * x1 * y1) + cy;
        x1 = temp;
        
        x2 = x1 * x1;
        y2 = y1 * y1;
        
        ++iteration;
      }    
    }

    color RenderPixel(int x, int y)
    {     
      imaginaryaxe deltaX = (fXmax - fXmin) / (intImageHeight - 1);  
      imaginaryaxe deltaY = (fYmax - fYmin) / (intImageWidth - 1);
      
      imaginaryaxe cx = fXmin + y * deltaX;
      imaginaryaxe cy = fYmin + x * deltaY;
      
      return ComputeArea(cx, cy);
   }

};

