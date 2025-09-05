#pragma once
#include "geom/Point.hpp"
#include <glm/mat3x3.hpp>
#include <memory>
#include <vector>

namespace egret {

    /**
     * Matrix类表示一个2D变换矩阵
     * 对应TypeScript: export class Matrix
     */
    class Matrix {
    public:
        /**
         * 矩阵元素
         * | a  c  tx |
         * | b  d  ty |
         * | 0  0  1  |
         */
        double a, b, c, d, tx, ty;

        /**
         * 构造函数
         */
        Matrix(double a = 1.0, double b = 0.0, double c = 0.0, double d = 1.0, double tx = 0.0, double ty = 0.0);
        
        /**
         * 拷贝构造函数
         */
        Matrix(const Matrix& other);
        
        /**
         * 从GLM mat3构造
         */
        Matrix(const glm::dmat3& mat);
        
        /**
         * 析构函数
         */
        ~Matrix() = default;

        // ========== 基础操作方法 ==========
        
        /**
         * 设置矩阵的所有元素
         */
        void setTo(double a, double b, double c, double d, double tx, double ty);
        
        /**
         * 复制另一个矩阵的值
         */
        void copyFrom(const Matrix& other);
        
        /**
         * 克隆当前矩阵
         */
        Matrix clone() const;
        
        /**
         * 重置为单位矩阵
         */
        void identity();
        
        /**
         * 检查是否为单位矩阵
         */
        bool isIdentity() const;
        
        /**
         * 检查两矩阵是否相等
         */
        bool equals(const Matrix& other) const;
        
        // ========== 变换操作 ==========
        
        /**
         * 应用平移变换
         */
        void translate(double dx, double dy);
        
        /**
         * 应用缩放变换
         */
        void scale(double sx, double sy);
        
        /**
         * 应用旋转变换（弧度）
         */
        void rotate(double angle);
        
        /**
         * 应用倾斜变换
         */
        void skew(double skewX, double skewY);
        
        /**
         * 前置乘法（当前矩阵 = other * 当前矩阵）
         */
        void prependMatrix(const Matrix& other);
        
        /**
         * 后置乘法（当前矩阵 = 当前矩阵 * other）
         */
        void appendMatrix(const Matrix& other);
        
        /**
         * 前置平移
         */
        void prependTranslation(double dx, double dy);
        
        /**
         * 前置缩放
         */
        void prependScale(double sx, double sy);
        
        /**
         * 前置旋转
         */
        void prependRotation(double angle);
        
        // ========== 高级操作 ==========
        
        /**
         * 计算矩阵的逆矩阵
         */
        Matrix invert() const;
        
        /**
         * 就地求逆
         */
        Matrix& invertSelf();
        
        /**
         * 获取行列式
         */
        double getDeterminant() const;
        
        // ========== 坐标变换 ==========
        
        /**
         * 变换点坐标
         */
        Point transformPoint(const Point& point) const;
        
        /**
         * 变换点坐标（就地修改）
         */
        void transformPointInPlace(Point& point) const;
        
        /**
         * 仅变换向量（忽略平移）
         */
        Point deltaTransformPoint(const Point& point) const;
        
        // ========== 属性提取 ==========
        
        /**
         * 获取矩阵元素访问器
         */
        double getA() const { return a; }
        double getB() const { return b; }
        double getC() const { return c; }
        double getD() const { return d; }
        double getTx() const { return tx; }
        double getTy() const { return ty; }
        
        /**
         * 获取X轴缩放系数
         */
        double getScaleX() const;
        
        /**
         * 获取Y轴缩放系数
         */
        double getScaleY() const;
        
        /**
         * 获取X轴倾斜角度（弧度）
         */
        double getSkewX() const;
        
        /**
         * 获取Y轴倾斜角度（弧度）
         */
        double getSkewY() const;
        
        /**
         * 获取旋转角度（弧度）
         */
        double getRotation() const;
        
        // ========== 复合变换设置 ==========
        
        /**
         * 根据缩放和倾斜更新矩阵
         */
        void updateScaleAndRotation(double scaleX, double scaleY, double skewX, double skewY);
        
        /**
         * 创建变换矩阵
         */
        static Matrix createTransform(double x, double y, double scaleX, double scaleY, double rotation, double skewX, double skewY, double anchorX, double anchorY);
        
        // ========== 矩阵运算 ==========
        
        /**
         * 矩阵乘法到结果矩阵
         */
        void multiplyInto(const Matrix& other, Matrix& result) const;
        
        /**
         * 前置乘法到结果矩阵
         */
        void preMultiplyInto(const Matrix& other, Matrix& result) const;
        
        // ========== 运算符重载 ==========
        
        Matrix& operator=(const Matrix& other);
        Matrix operator*(const Matrix& other) const;
        Matrix& operator*=(const Matrix& other);
        bool operator==(const Matrix& other) const;
        bool operator!=(const Matrix& other) const;
        
        // ========== GLM集成 ==========
        
        /**
         * 转换为GLM mat3
         */
        glm::dmat3 toGLM() const;
        
        /**
         * 从GLM mat3更新
         */
        void fromGLM(const glm::dmat3& mat);
        
        /**
         * 转换操作符
         */
        operator glm::dmat3() const;
        
        // ========== 对象池支持 ==========
        
        /**
         * 创建Matrix对象（使用对象池）
         */
        static std::shared_ptr<Matrix> create(double a = 1.0, double b = 0.0, double c = 0.0, double d = 1.0, double tx = 0.0, double ty = 0.0);
        
        /**
         * 回收Matrix对象到对象池
         */
        static void release(std::shared_ptr<Matrix> matrix);

    private:
        // 对象池相关
        static const size_t POOL_SIZE = 50;
        static thread_local std::vector<std::shared_ptr<Matrix>> s_pool;
        static thread_local size_t s_poolIndex;
        
        // 辅助方法
        void updateFromTransform(double scaleX, double scaleY, double skewX, double skewY);
    };

} // namespace egret