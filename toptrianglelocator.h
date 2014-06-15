#ifndef TOPTRIANGLELOCATOR_H
#define TOPTRIANGLELOCATOR_H

#include"daddy.h"

class TopTriangleLocator : public Daddy
{
    Q_OBJECT
    public:
        explicit TopTriangleLocator(QWidget *parent=0);
        ~TopTriangleLocator();
        enum Azimuth{A_NO=0u,A_FIRST=10u,A_SECOND=30u};
        enum Range{R_NO=0u,R_FIRST=5u,R_SECOND=10u};
        enum Scale{S_SMALL=20u,S_MIDDLE=30u,S_LARGE=60u};
        enum WorkMode{WM_AKT=0u,WM_PASS,WM_SDC};

        Azimuth GetCurrentAzimuthMode(void)const;
        void SetCurrentAzimuthMode(const Azimuth);
        Range GetCurrentRangeMode(void)const;
        void SetCurrentRangeMode(const Range);
        Scale GetCurrentScaleMode(void)const;
        void SetCurrentScaleMode(const Scale);
        WorkMode GetCurrentWorkMode(void)const;
        void SetCurrentWorkMode(const WorkMode);

    signals:

    public slots:
    protected:
        Azimuth azimuth=Azimuth::A_NO;
        Range range=Range::R_NO;
        Scale scale=Scale::S_SMALL;
        WorkMode work_mode=WorkMode::WM_PASS;
};

#endif // TOPTRIANGLELOCATOR_H
