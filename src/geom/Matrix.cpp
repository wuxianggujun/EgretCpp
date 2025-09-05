#include "geom/Matrix.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

namespace egret {

    // 对象池静态成员定义
    thread_local std::vector<std::shared_ptr<Matrix>> Matrix::s_pool;
    thread_local size_t Matrix::s_poolIndex = 0;

    Matrix::Matrix(double a, double b, double c, double d, double tx, double ty)
        : a(a), b(b), c(c), d(d), tx(tx), ty(ty) {
    }

    Matrix::Matrix(const Matrix& other)
        : a(other.a), b(other.b), c(other.c), d(other.d), tx(other.tx), ty(other.ty) {
    }

    Matrix::Matrix(const glm::dmat3& mat) {
        fromGLM(mat);
    }

    void Matrix::setTo(double a, double b, double c, double d, double tx, double ty) {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->tx = tx;
        this->ty = ty;
    }

    void Matrix::copyFrom(const Matrix& other) {
        a = other.a;
        b = other.b;
        c = other.c;
        d = other.d;
        tx = other.tx;
        ty = other.ty;
    }

    Matrix Matrix::clone() const {
        return Matrix(a, b, c, d, tx, ty);
    }

    void Matrix::identity() {
        setTo(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    }

    bool Matrix::isIdentity() const {
        const double epsilon = 1e-10;
        return std::abs(a - 1.0) < epsilon && std::abs(b) < epsilon &&
               std::abs(c) < epsilon && std::abs(d - 1.0) < epsilon &&
               std::abs(tx) < epsilon && std::abs(ty) < epsilon;
    }

    bool Matrix::equals(const Matrix& other) const {
        const double epsilon = 1e-10;
        return std::abs(a - other.a) < epsilon && std::abs(b - other.b) < epsilon &&
               std::abs(c - other.c) < epsilon && std::abs(d - other.d) < epsilon &&
               std::abs(tx - other.tx) < epsilon && std::abs(ty - other.ty) < epsilon;
    }

    void Matrix::translate(double dx, double dy) {
        tx += dx;
        ty += dy;
    }

    void Matrix::scale(double sx, double sy) {
        a *= sx;
        b *= sx;
        c *= sy;
        d *= sy;
        tx *= sx;
        ty *= sy;
    }

    void Matrix::rotate(double angle) {
        double cos_a = std::cos(angle);
        double sin_a = std::sin(angle);
        
        double temp_a = a * cos_a - b * sin_a;
        double temp_b = a * sin_a + b * cos_a;
        double temp_c = c * cos_a - d * sin_a;
        double temp_d = c * sin_a + d * cos_a;
        double temp_tx = tx * cos_a - ty * sin_a;
        double temp_ty = tx * sin_a + ty * cos_a;
        
        a = temp_a;
        b = temp_b;
        c = temp_c;
        d = temp_d;
        tx = temp_tx;
        ty = temp_ty;
    }

    void Matrix::skew(double skewX, double skewY) {
        double tanX = std::tan(skewX);
        double tanY = std::tan(skewY);
        
        double temp_a = a + b * tanY;
        double temp_b = a * tanX + b;
        double temp_c = c + d * tanY;
        double temp_d = c * tanX + d;
        double temp_tx = tx + ty * tanY;
        double temp_ty = tx * tanX + ty;
        
        a = temp_a;
        b = temp_b;
        c = temp_c;
        d = temp_d;
        tx = temp_tx;
        ty = temp_ty;
    }

    void Matrix::prependMatrix(const Matrix& other) {
        double temp_a = other.a * a + other.b * c;
        double temp_b = other.a * b + other.b * d;
        double temp_c = other.c * a + other.d * c;
        double temp_d = other.c * b + other.d * d;
        double temp_tx = other.a * tx + other.b * ty + other.tx;
        double temp_ty = other.c * tx + other.d * ty + other.ty;
        
        a = temp_a;
        b = temp_b;
        c = temp_c;
        d = temp_d;
        tx = temp_tx;
        ty = temp_ty;
    }

    void Matrix::appendMatrix(const Matrix& other) {
        double temp_a = a * other.a + b * other.c;
        double temp_b = a * other.b + b * other.d;
        double temp_c = c * other.a + d * other.c;
        double temp_d = c * other.b + d * other.d;
        double temp_tx = a * other.tx + b * other.ty + tx;
        double temp_ty = c * other.tx + d * other.ty + ty;
        
        a = temp_a;
        b = temp_b;
        c = temp_c;
        d = temp_d;
        tx = temp_tx;
        ty = temp_ty;
    }

    void Matrix::prependTranslation(double dx, double dy) {
        tx += a * dx + c * dy;
        ty += b * dx + d * dy;
    }

    void Matrix::prependScale(double sx, double sy) {
        a *= sx;
        b *= sx;
        c *= sy;
        d *= sy;
    }

    void Matrix::prependRotation(double angle) {
        double cos_a = std::cos(angle);
        double sin_a = std::sin(angle);
        
        double temp_a = cos_a * a + sin_a * c;
        double temp_b = cos_a * b + sin_a * d;
        double temp_c = -sin_a * a + cos_a * c;
        double temp_d = -sin_a * b + cos_a * d;
        
        a = temp_a;
        b = temp_b;
        c = temp_c;
        d = temp_d;
    }

    Matrix Matrix::invert() const {
        double det = getDeterminant();
        if (std::abs(det) < 1e-10) {
            return Matrix(); // 返回单位矩阵
        }
        
        double inv_det = 1.0 / det;
        return Matrix(
            d * inv_det,
            -b * inv_det,
            -c * inv_det,
            a * inv_det,
            (c * ty - d * tx) * inv_det,
            (b * tx - a * ty) * inv_det
        );
    }

    Matrix& Matrix::invertSelf() {
        *this = invert();
        return *this;
    }

    double Matrix::getDeterminant() const {
        return a * d - b * c;
    }

    Point Matrix::transformPoint(const Point& point) const {
        double x = point.getX();
        double y = point.getY();
        return Point(
            a * x + c * y + tx,
            b * x + d * y + ty
        );
    }

    void Matrix::transformPointInPlace(Point& point) const {
        double x = point.getX();
        double y = point.getY();
        point.setX(a * x + c * y + tx);
        point.setY(b * x + d * y + ty);
    }

    Point Matrix::deltaTransformPoint(const Point& point) const {
        double x = point.getX();
        double y = point.getY();
        return Point(
            a * x + c * y,
            b * x + d * y
        );
    }

    double Matrix::getScaleX() const {
        return std::sqrt(a * a + b * b);
    }

    double Matrix::getScaleY() const {
        return std::sqrt(c * c + d * d);
    }

    double Matrix::getSkewX() const {
        return std::atan2(b, a);
    }

    double Matrix::getSkewY() const {
        return std::atan2(-c, d);
    }

    double Matrix::getRotation() const {
        return getSkewX();
    }

    void Matrix::updateScaleAndRotation(double scaleX, double scaleY, double skewX, double skewY) {
        updateFromTransform(scaleX, scaleY, skewX, skewY);
    }

    void Matrix::updateFromTransform(double scaleX, double scaleY, double skewX, double skewY) {
        a = std::cos(skewY) * scaleX;
        b = std::sin(skewY) * scaleX;
        c = -std::sin(skewX) * scaleY;
        d = std::cos(skewX) * scaleY;
    }

    Matrix Matrix::createTransform(double x, double y, double scaleX, double scaleY, double rotation, double skewX, double skewY, double anchorX, double anchorY) {
        Matrix matrix;
        
        // 应用锚点偏移
        if (anchorX != 0.0 || anchorY != 0.0) {
            matrix.tx = -anchorX;
            matrix.ty = -anchorY;
        }
        
        // 应用缩放和倾斜
        matrix.updateScaleAndRotation(scaleX, scaleY, skewX + rotation, skewY + rotation);
        
        // 应用位置
        matrix.tx += x;
        matrix.ty += y;
        
        return matrix;
    }

    void Matrix::multiplyInto(const Matrix& other, Matrix& result) const {
        result.a = a * other.a + b * other.c;
        result.b = a * other.b + b * other.d;
        result.c = c * other.a + d * other.c;
        result.d = c * other.b + d * other.d;
        result.tx = a * other.tx + b * other.ty + tx;
        result.ty = c * other.tx + d * other.ty + ty;
    }

    void Matrix::preMultiplyInto(const Matrix& other, Matrix& result) const {
        result.a = other.a * a + other.b * c;
        result.b = other.a * b + other.b * d;
        result.c = other.c * a + other.d * c;
        result.d = other.c * b + other.d * d;
        result.tx = other.a * tx + other.b * ty + other.tx;
        result.ty = other.c * tx + other.d * ty + other.ty;
    }

    Matrix& Matrix::operator=(const Matrix& other) {
        if (this != &other) {
            copyFrom(other);
        }
        return *this;
    }

    Matrix Matrix::operator*(const Matrix& other) const {
        Matrix result;
        multiplyInto(other, result);
        return result;
    }

    Matrix& Matrix::operator*=(const Matrix& other) {
        appendMatrix(other);
        return *this;
    }

    bool Matrix::operator==(const Matrix& other) const {
        return equals(other);
    }

    bool Matrix::operator!=(const Matrix& other) const {
        return !equals(other);
    }

    glm::dmat3 Matrix::toGLM() const {
        return glm::dmat3(
            a,  b,  0.0,
            c,  d,  0.0,
            tx, ty, 1.0
        );
    }

    void Matrix::fromGLM(const glm::dmat3& mat) {
        a = mat[0][0];
        b = mat[0][1];
        c = mat[1][0];
        d = mat[1][1];
        tx = mat[2][0];
        ty = mat[2][1];
    }

    Matrix::operator glm::dmat3() const {
        return toGLM();
    }

    // 对象池实现
    std::shared_ptr<Matrix> Matrix::create(double a, double b, double c, double d, double tx, double ty) {
        if (s_pool.empty()) {
            s_pool.reserve(POOL_SIZE);
        }
        
        if (s_poolIndex < s_pool.size()) {
            auto matrix = s_pool[s_poolIndex++];
            matrix->setTo(a, b, c, d, tx, ty);
            return matrix;
        }
        
        auto matrix = std::make_shared<Matrix>(a, b, c, d, tx, ty);
        if (s_pool.size() < POOL_SIZE) {
            s_pool.push_back(matrix);
            s_poolIndex++;
        }
        return matrix;
    }

    void Matrix::release(std::shared_ptr<Matrix> matrix) {
        if (!matrix || s_poolIndex == 0) {
            return;
        }
        
        // 重置为单位矩阵并放回池中
        matrix->identity();
        s_poolIndex--;
        
        // 将该矩阵移到池的开头位置
        if (s_poolIndex < s_pool.size()) {
            std::swap(s_pool[s_poolIndex], s_pool.back());
        }
    }

} // namespace egret