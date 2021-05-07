#include "processimage.h"


Image::Image()
{
  ImageLoad();
  Matrix = new int*[height];
  CopyMatrix = new int*[height];
  InitialMatrix = new int*[height];
  for(int i = 0; i < height; i++){
      Matrix[i] = new int[width];
      CopyMatrix[i] = new int[width];
      InitialMatrix[i] = new int[width];
    }
  ReadImage();
}

void Image::ImageLoad()
{
  if(!image.load(ImageProcessing::WayToFile + ImageProcessing::NameOfPicture))
    return;
  height = image.height();
  width = image.width();
}


void Image::ReadImage()
{
  for(int row = 0; row < height; row++)
    for(int col = 0;col < width; col++)
      {
       int n = image.pixelColor(col,row).value();
            Matrix[row][col] = (n > 127 ? 0 : 1);
            CopyMatrix[row][col] = Matrix[row][col];
            InitialMatrix[row][col] = Matrix[row][col];
      }
  image = image.convertToFormat(QImage::Format_Mono);
}

void Image::DoCopyMatrix()
{
  for(int row = 0; row < height; row++)
    for(int col = 0; col < width; col++)
      CopyMatrix[row][col] = Matrix[row][col];
}

Image::~Image(){
  for(int i = 0; i < height; i++)
    {
      delete[] CopyMatrix[i];
      delete[] Matrix[i];
      delete[] InitialMatrix[i];
    }
  delete[] Matrix;
  delete[] CopyMatrix;
  delete[] InitialMatrix;
}



Mask::Mask()
{
  ReadMaskFromFile();
  Matrix = new int*[height];
  for(int i = 0; i < height; i++)
    Matrix[i] = new int[width];
  FillMask();
}


Mask::~Mask()
{
  for(int i = 0;i < height;i++)
    delete[]  Matrix[i];
  delete[] Matrix;
}

void Mask::ReadMaskFromFile()
{
  QFile File(ImageProcessing::WayToFile + ImageProcessing::NameOfMask);
  if(!File.open(QIODevice::ReadOnly))
    return;
  QByteArray text = File.readAll();
  CoordinateCentreOne = text[0] - 48;
  CoordinateCentreTwo = text [2] - 48;
  text.remove(0,5);
  QString array = text;
  list = array.split(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
  height = list.size();
  list = array.split(QRegExp("[\r\n ]"), Qt::SkipEmptyParts);
  width = list.size()/height;
}

void Mask::FillMask()
{
  for(int i = 0; i < height; i++)
      for(int j = 0; j < width; j++)
        Matrix[i][j] = list[ i * width + j].toInt();
}

ImageProcessing::ImageProcessing()
{
  MaskCoorDCentOne  = mask.GetCoordOne();
  MaskCoordCentTwo = mask.GetCoordTwo();
  WidthImage = img.GetWidth();
  HeightImage = img.GetHeight();
  WidthMask = mask.GetWidth();
  HeightMask = mask.GetHeight();
  OutlineMatrix = new int*[HeightImage];
  for(int i=0;i < HeightImage;i++)
    OutlineMatrix[i] = new int[WidthImage];
}

ImageProcessing::~ImageProcessing()
{
  for(int i = 0;i < HeightImage;i++)
    delete[] OutlineMatrix[i];
  delete[] OutlineMatrix;
}

void ImageProcessing::Dilatation(int DepthOfDilatation)
{
  if(states != Outline)
    states = Dilat;
  while(DepthOfDilatation--){
      for(int row = 1; row < HeightImage - 1; row++){
          for(int col = 1; col < WidthImage - 1; col++){
            if(img.CopyMatrix[row][col] == mask.Matrix[MaskCoorDCentOne][MaskCoordCentTwo])
              DelatationFillNewImage({row,col});
            }
        }
      img.DoCopyMatrix();
    }
  FillNewImage();
}

void ImageProcessing::DefineShift(QPair<int,int>& CoordsPoint)
{
  CoordsPoint.first -=  MaskCoorDCentOne;
  CoordsPoint.second -= MaskCoordCentTwo;
}

void ImageProcessing::DelatationFillNewImage(QPair<int,int> CoordsPoint)
{
  DefineShift(CoordsPoint);
  for(int x = CoordsPoint.first,iter1 = 0; iter1 < HeightMask; x++, iter1++)
    for(int y = CoordsPoint.second,iter2 = 0; iter2 < WidthMask; y++, iter2++)
      if(mask.Matrix[iter1][iter2] == 1)
        img.Matrix[x][y] = mask.Matrix[iter1][iter2];
}

void ImageProcessing::Erosion(int DepthOfErosion)
{
  if(states != Outline)
    states = Eros;
  while(DepthOfErosion--){
      for(int row = 1; row < HeightImage - 1; row++)
        for(int col = 1; col < WidthImage - 1; col++){
            if(img.CopyMatrix[row][col] == mask.Matrix[MaskCoorDCentOne][MaskCoordCentTwo]){
                if(MaskMatched({row,col}) == false)
                  img.Matrix[row][col] = 0;
              }
          }
      img.DoCopyMatrix();
    }
  FillNewImage();
}

bool ImageProcessing::MaskMatched(QPair<int, int> CoordsPoint){
  DefineShift(CoordsPoint);
  for(int x = CoordsPoint.first,iter1 = 0; iter1 < HeightMask; x++, iter1++)
    for(int y = CoordsPoint.second,iter2 = 0; iter2 < WidthMask; y++, iter2++)
      if(mask.Matrix[iter1][iter2] == 1)
        {
          if(img.CopyMatrix[x][y] != mask.Matrix[iter1][iter2])
            return false;
        }
  return true;
}

void ImageProcessing::outline(int WidthOutline)
{
  states = Outline;
  int eros = WidthOutline / 2;
  Erosion(eros);
  for(int row = 0 ; row < HeightImage; row++)
    for(int col = 0; col < WidthImage; col++)
        OutlineMatrix[row][col] = img.Matrix[row][col];
  clear();
  Erosion(1);
  Dilatation(WidthOutline - eros + 1);
  int pix;
  for(int row = 0 ; row < HeightImage; row++)
    for(int col = 0; col < WidthImage; col++){
         pix = img.Matrix[row][col] - OutlineMatrix[row][col];
         img.image.setPixel(col,row, pix >= 0 ? pix : 0);
      }
  clear();
}


void ImageProcessing::FillNewImage()
{
  for(int row = 0; row < HeightImage; row++)
    for(int col = 0; col < WidthImage; col++){
        if(img.Matrix[row][col] == 1)
          {
            img.image.setPixel(col,row,1);
            continue;
          }
        img.image.setPixel(col,row,0);
      }
}

void ImageProcessing::SaveNewImage(){
  if(states == Dilat){
      img.image.save(WayToFile + "/Dilatation.png");
      clear();
    }
  if(states == Eros){
      img.image.save(WayToFile + "/Erosion.png");
      clear();
    }
  if(states == Outline){
      img.image.save(WayToFile+ "/Outline.png");
      clear();
    }
}

void ImageProcessing::clear()
{
   for(int row = 0; row < HeightImage; row++)
     for(int col = 0; col < WidthImage; col++){
        img.Matrix[row][col] = img.InitialMatrix[row][col];
        img.CopyMatrix[row][col] = img.InitialMatrix[row][col];
       }
}
