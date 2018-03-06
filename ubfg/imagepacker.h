#ifndef IMAGEPACKER_H
#define IMAGEPACKER_H

#include <QObject>
#include <QImage>

struct packedImage
{
    QImage img;
    QRect rc;
    QRect crop;
    bool border;
    bool merged;
    bool rotated;
    int textureId;
    int charWidth;
    int bearing;
    QChar ch;
};

class ImagePacker : public QObject
{
public:
    ImagePacker();
    bool compareImages(QImage* img1, QImage* img2, int* i, int *j);
    QList<QPoint> pack(QList<packedImage> *images, int heuristic, uint w = 128, uint h = 128, bool allowRotate = false);
    void crop(QList<packedImage*> *images);
    void sort(QList<packedImage*> *images);
    int compare;
    int area;
    int missingChars;
    int mergedChars;
    bool ltr, trim, merge, mergeBF;
    unsigned int borderTop, borderBottom, borderLeft, borderRight;
    int neededArea;
    int sortOrder;
    enum {GUILLOTINE, MAXRECTS}; //method
    enum {NONE, TL, BAF, BSSF, BLSF, MINW, MINH}; //heuristic
    enum {SORT_NONE, WIDTH, HEIGHT, SORT_AREA, SORT_MAX, SORT_NUM}; //sort
    enum {NEVER, ONLY_WHEN_NEEDED, H2_WIDTH_H, WIDTH_GREATHER_HEIGHT, WIDTH_GREATHER_2HEIGHT, W2_HEIGHT_W, HEIGHT_GREATHER_WIDTH, HEIGHT_GREATHER_2WIDTH, ROTATION_NUM}; //rotation
};




#endif // IMAGEPACKER_H
