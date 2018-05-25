#ifndef NO_COPY_H
#define NO_COPY_H

namespace utility
{
    struct NoCopy
    {
    public:
        NoCopy() = default;
        NoCopy(const NoCopy &) = delete;
        virtual ~NoCopy() = default;
    };

    struct NoMove
    {
    public:
        NoMove() = default;
        NoMove(const NoMove &&) = delete;
        virtual ~NoMove() = default;
    };

    struct NoAssign
    {
    public:
        NoAssign() = default;
        NoAssign &operator=(const NoAssign &) = delete;
        NoAssign &operator=(const NoAssign &&) = delete;
        virtual ~NoAssign() = default;
    };

    struct NoCopyAssign : public virtual NoCopy, public virtual NoAssign
    {
    public:
        NoCopyAssign() = default;
        virtual ~NoCopyAssign() override = default;
    };
    using NoAssignCopy = NoCopyAssign;


    struct NoCopyAssignMove : public virtual NoCopy, public virtual NoAssign, public virtual NoMove
    {
    public:
        NoCopyAssignMove() = default;
        virtual ~NoCopyAssignMove() override = default;
    };
}

#endif // NO_COPY_H
