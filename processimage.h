#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H
#include <iostream>
#include <QCoreApplication>
#include <QString>
#include <QFile>
#include <QImage>
#include <QRegExp>
#include <QPair>

enum States{
  Eros,
  Dilat,
  Outline
};

class Image
{
private:
  int height; //высота картинки
  int width; //ширина картинки
public:
  QImage image;
  int** Matrix;
  int** CopyMatrix;
  int** InitialMatrix;
  Image();
  ~Image();
  void ImageLoad();
  void ReadImage();
  void DoCopyMatrix();
  int GetWidth(){return width;}
  int GetHeight(){return height;}
};

class Mask
{
private:
  int CoordinateCentreOne;
  int CoordinateCentreTwo;
  QStringList list;
  int height; //cтроки матрицы
  int width; //столбцы матрицы
public:
  int** Matrix;
  Mask();
  ~Mask();
 void ReadMaskFromFile();
 void FillMask();
 int** GetMask(){return Matrix;};
 int GetWidth(){return width;}
 int GetHeight(){return height;}
 int GetCoordOne(){return CoordinateCentreOne;}
 int GetCoordTwo(){return CoordinateCentreTwo;}

};

class ImageProcessing
{
private:
  int MaskCoorDCentOne;
  int MaskCoordCentTwo;
  int WidthImage;
  int HeightImage;
  int WidthMask;
  int HeightMask;
  QPair<int,int> Coords;
  Image img;
  Mask mask;
  int** OutlineMatrix;
  States states; // что сейчас выполняется: эрозия, дилатация или контур
public:
  static QString NameOfMask, NameOfPicture, WayToFile;

  ImageProcessing();
  ~ImageProcessing();
//Для дилатаци....
  void DefineShift(QPair<int,int>&);
  void Dilatation(int);
  void DelatationFillNewImage(QPair<int,int>);
//Для эрозии...
  void Erosion(int);
  bool MaskMatched(QPair<int,int>);
//Для контура...
  void outline(int);
//Сохранение картинки....
  void FillNewImage();
  void SaveNewImage();
  void clear();
};

#endif // IMAGEPROCESSING_H
