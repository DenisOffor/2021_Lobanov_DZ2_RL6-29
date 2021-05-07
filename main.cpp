#include <QCoreApplication>
#include "processimage.h"
#include <QDir>

QString FindWay()
{
  QDir dir;
  dir.absolutePath();
  dir.cdUp();
  return dir.filePath("2021_Lobanov_DZ2_RL6-29");
}

QString ImageProcessing::WayToFile = FindWay();
QString ImageProcessing::NameOfMask = "/Mask.txt";
QString ImageProcessing::NameOfPicture = "/Picture.png";

int main(int argc, char *argv[])
{
 int DepthOfDilatation = atoi(argv[1]);  //кол-во дилатаций
 int DepthOfErosion = atoi(argv[2]); //кол-во эрозий
 int WidthOutline = atoi(argv[3]); //ширина контура в пикселях
 ImageProcessing process;
 process.Erosion(DepthOfErosion);
 process.Dilatation(DepthOfDilatation);
 process.outline(WidthOutline);
 return 0;
}

