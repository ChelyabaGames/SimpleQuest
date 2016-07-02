#ifndef Geom_h
#define Geom_h

// Integer 2D point
struct Point2DI {
    int x;
    int y;

    Point2DI& operator+=(const Point2DI& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Point2DI& operator-=(const Point2DI& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
};

inline Point2DI operator+(const Point2DI& lhs, const Point2DI& rhs)
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}

inline Point2DI operator-(const Point2DI& lhs, const Point2DI& rhs)
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}

// Integer size
struct SizeI {
    int width;
    int height;
};

// Integer rectangle
union RectI {
    struct {
        int left;
        int top;
        int right;
        int bottom;
    };

    struct {
        Point2DI topLeft;
        Point2DI bottomRight;
    };

    bool isValid() const
    {
        return right >= left
            && bottom >= top;
    }

    Point2DI pos() const
    {
        return topLeft;
    }

    void setPos(const Point2DI& pos)
    {
        bottomRight += (pos - topLeft);
        topLeft = pos;
    }

    SizeI size() const
    {
        return { right - left, bottom - top };
    }

    void setSize(const SizeI& size)
    {
        right = left + size.width;
        bottom = top + size.height;
    }
};

#endif // Geom_h
