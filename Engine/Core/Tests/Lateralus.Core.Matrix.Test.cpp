#include <gtest/gtest.h>

import Lateralus.Core.Matrix;
import Lateralus.Core.Math;

using namespace Lateralus;

namespace Lateralus::Core::Tests
{
TEST(Matrix4x4Test, DefaultConstructor)
{
    Lateralus::Core::Matrix4x4 matrix;
    // Assert that the diagonal elements are set to 1 and others to 0
    ASSERT_FLOAT_EQ(matrix.r0.x, 1.0f);
    ASSERT_FLOAT_EQ(matrix.r1.y, 1.0f);
    ASSERT_FLOAT_EQ(matrix.r2.z, 1.0f);
    ASSERT_FLOAT_EQ(matrix.r3.w, 1.0f);

    ASSERT_FLOAT_EQ(matrix.r0.y, 0.0f);
    ASSERT_FLOAT_EQ(matrix.r0.z, 0.0f);
    ASSERT_FLOAT_EQ(matrix.r0.w, 0.0f);

    ASSERT_FLOAT_EQ(matrix.r1.x, 0.0f);
    ASSERT_FLOAT_EQ(matrix.r1.z, 0.0f);
    ASSERT_FLOAT_EQ(matrix.r1.w, 0.0f);

    ASSERT_FLOAT_EQ(matrix.r2.x, 0.0f);
    ASSERT_FLOAT_EQ(matrix.r2.y, 0.0f);
    ASSERT_FLOAT_EQ(matrix.r2.w, 0.0f);

    ASSERT_FLOAT_EQ(matrix.r3.x, 0.0f);
    ASSERT_FLOAT_EQ(matrix.r3.y, 0.0f);
    ASSERT_FLOAT_EQ(matrix.r3.z, 0.0f);
}

TEST(Matrix4x4Test, IndividualElementsConstructor)
{
    Lateralus::Core::Matrix4x4 matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
                                      11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
    // Assert that the elements are set correctly
    ASSERT_FLOAT_EQ(matrix.r0.x, 1.0f);
    ASSERT_FLOAT_EQ(matrix.r0.y, 2.0f);
    ASSERT_FLOAT_EQ(matrix.r0.z, 3.0f);
    ASSERT_FLOAT_EQ(matrix.r0.w, 4.0f);
    ASSERT_FLOAT_EQ(matrix.r3.w, 16.0f);
}

TEST(Matrix4x4Test, MatrixMultiplicationOperator)
{
    Lateralus::Core::Matrix4x4 matrix1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
                                       11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

    Lateralus::Core::Matrix4x4 matrix2(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                       2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f);

    Lateralus::Core::Matrix4x4 expected(2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 18.0f,
                                        20.0f, 22.0f, 24.0f, 26.0f, 28.0f, 30.0f, 32.0f);

    Lateralus::Core::Matrix4x4 result = matrix1 * matrix2;

    ASSERT_FLOAT_EQ(result.r0.x, expected.r0.x);
    ASSERT_FLOAT_EQ(result.r0.y, expected.r0.y);
    ASSERT_FLOAT_EQ(result.r0.z, expected.r0.z);
    ASSERT_FLOAT_EQ(result.r0.w, expected.r0.w);

    ASSERT_FLOAT_EQ(result.r1.x, expected.r1.x);
    ASSERT_FLOAT_EQ(result.r1.y, expected.r1.y);
    ASSERT_FLOAT_EQ(result.r1.z, expected.r1.z);
    ASSERT_FLOAT_EQ(result.r1.w, expected.r1.w);

    ASSERT_FLOAT_EQ(result.r2.x, expected.r2.x);
    ASSERT_FLOAT_EQ(result.r2.y, expected.r2.y);
    ASSERT_FLOAT_EQ(result.r2.z, expected.r2.z);
    ASSERT_FLOAT_EQ(result.r2.w, expected.r2.w);

    ASSERT_FLOAT_EQ(result.r3.x, expected.r3.x);
    ASSERT_FLOAT_EQ(result.r3.y, expected.r3.y);
    ASSERT_FLOAT_EQ(result.r3.z, expected.r3.z);
    ASSERT_FLOAT_EQ(result.r3.w, expected.r3.w);
}

TEST(Matrix4x4Test, MatrixVectorMultiplicationOperator)
{
    // Initialize matrix for testing
    Lateralus::Core::Matrix4x4 matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
                                      11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

    // Initialize vector for testing
    Lateralus::Core::Vector4 vector(1.0f, 2.0f, 3.0f, 4.0f);

    // Calculate the expected result manually
    Lateralus::Core::Vector4 expected(30.0f, 70.0f, 110.0f, 150.0f);

    // Perform matrix-vector multiplication
    Lateralus::Core::Vector4 result = matrix * vector;

    // Assert that each element of the result vector matches the expected result
    ASSERT_FLOAT_EQ(result.x, expected.x);
    ASSERT_FLOAT_EQ(result.y, expected.y);
    ASSERT_FLOAT_EQ(result.z, expected.z);
    ASSERT_FLOAT_EQ(result.w, expected.w);
}

TEST(Matrix4x4Test, Transpose)
{
    Lateralus::Core::Matrix4x4 matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
                                      11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

    Lateralus::Core::Matrix4x4 transposed = matrix.Transpose();

    // Assert that the matrix is transposed correctly
    ASSERT_FLOAT_EQ(transposed.r0.x, matrix.r0.x);
    ASSERT_FLOAT_EQ(transposed.r0.y, matrix.r1.x);
    ASSERT_FLOAT_EQ(transposed.r0.z, matrix.r2.x);
    ASSERT_FLOAT_EQ(transposed.r0.w, matrix.r3.x);

    ASSERT_FLOAT_EQ(transposed.r1.x, matrix.r0.y);
    ASSERT_FLOAT_EQ(transposed.r1.y, matrix.r1.y);
    ASSERT_FLOAT_EQ(transposed.r1.z, matrix.r2.y);
    ASSERT_FLOAT_EQ(transposed.r1.w, matrix.r3.y);

    ASSERT_FLOAT_EQ(transposed.r2.x, matrix.r0.z);
    ASSERT_FLOAT_EQ(transposed.r2.y, matrix.r1.z);
    ASSERT_FLOAT_EQ(transposed.r2.z, matrix.r2.z);
    ASSERT_FLOAT_EQ(transposed.r2.w, matrix.r3.z);

    ASSERT_FLOAT_EQ(transposed.r3.x, matrix.r0.w);
    ASSERT_FLOAT_EQ(transposed.r3.y, matrix.r1.w);
    ASSERT_FLOAT_EQ(transposed.r3.z, matrix.r2.w);
    ASSERT_FLOAT_EQ(transposed.r3.w, matrix.r3.w);

    // Transposing twice should get back the original matrix
    Lateralus::Core::Matrix4x4 original = transposed.Transpose();
    ASSERT_TRUE(original.RoughlyEquals(matrix));
}

// Test Inverse operation
TEST(Matrix4x4Test, Inverse)
{
    Lateralus::Core::Matrix4x4 original(2.0f, 1.0f, 3.0f, 0.0f, 1.0f, 3.0f, 2.0f, 1.0f, 0.0f, -2.0f,
                                        1.0f, 2.0f, 1.0f, 0.0f, -1.0f, 1.0f);

    Lateralus::Core::Matrix4x4 expectedInverse(
        0.357143f, -0.214286f, -0.142857f, 0.5f, -0.142857f, 0.285714f, -0.142857f, 0.0f, 0.142857f,
        0.047619f, 0.142857f, -0.333333f, -0.214286f, 0.261905f, 0.285714f, 0.166667f);

    Lateralus::Core::Matrix4x4 inverse = original.Inverse();

    ASSERT_NEAR(inverse.r0.x, expectedInverse.r0.x, 1e-5);
    ASSERT_NEAR(inverse.r0.y, expectedInverse.r0.y, 1e-5);
    ASSERT_NEAR(inverse.r0.z, expectedInverse.r0.z, 1e-5);
    ASSERT_NEAR(inverse.r0.w, expectedInverse.r0.w, 1e-5);

    ASSERT_NEAR(inverse.r1.x, expectedInverse.r1.x, 1e-5);
    ASSERT_NEAR(inverse.r1.y, expectedInverse.r1.y, 1e-5);
    ASSERT_NEAR(inverse.r1.z, expectedInverse.r1.z, 1e-5);
    ASSERT_NEAR(inverse.r1.w, expectedInverse.r1.w, 1e-5);

    ASSERT_NEAR(inverse.r2.x, expectedInverse.r2.x, 1e-5);
    ASSERT_NEAR(inverse.r2.y, expectedInverse.r2.y, 1e-5);
    ASSERT_NEAR(inverse.r2.z, expectedInverse.r2.z, 1e-5);
    ASSERT_NEAR(inverse.r2.w, expectedInverse.r2.w, 1e-5);

    ASSERT_NEAR(inverse.r3.x, expectedInverse.r3.x, 1e-5);
    ASSERT_NEAR(inverse.r3.y, expectedInverse.r3.y, 1e-5);
    ASSERT_NEAR(inverse.r3.z, expectedInverse.r3.z, 1e-5);
    ASSERT_NEAR(inverse.r3.w, expectedInverse.r3.w, 1e-5);
}

TEST(Matrix4x4Test, Identity)
{
    Lateralus::Core::Matrix4x4 identity;

    ASSERT_FLOAT_EQ(identity.r0.x, 1.0f);
    ASSERT_FLOAT_EQ(identity.r0.y, 0.0f);
    ASSERT_FLOAT_EQ(identity.r0.z, 0.0f);
    ASSERT_FLOAT_EQ(identity.r0.w, 0.0f);

    ASSERT_FLOAT_EQ(identity.r1.x, 0.0f);
    ASSERT_FLOAT_EQ(identity.r1.y, 1.0f);
    ASSERT_FLOAT_EQ(identity.r1.z, 0.0f);
    ASSERT_FLOAT_EQ(identity.r1.w, 0.0f);

    ASSERT_FLOAT_EQ(identity.r2.x, 0.0f);
    ASSERT_FLOAT_EQ(identity.r2.y, 0.0f);
    ASSERT_FLOAT_EQ(identity.r2.z, 1.0f);
    ASSERT_FLOAT_EQ(identity.r2.w, 0.0f);

    ASSERT_FLOAT_EQ(identity.r3.x, 0.0f);
    ASSERT_FLOAT_EQ(identity.r3.y, 0.0f);
    ASSERT_FLOAT_EQ(identity.r3.z, 0.0f);
    ASSERT_FLOAT_EQ(identity.r3.w, 1.0f);
}

TEST(Matrix4x4Test, Determinant)
{
    Lateralus::Core::Matrix4x4 matrix(
        2.0f, 1.0f, 3.0f, 0.0f,
        1.0f, 3.0f, 2.0f, 1.0f,
        0.0f, 2.0f, 1.0f, 0.0f,
        1.0f, 2.0f, 1.0f, 0.0f
    );

    float expectedDeterminant = -5.0f;

    float determinant = matrix.Determinant();

    ASSERT_NEAR(determinant, expectedDeterminant, 1e-5);
}


TEST(Matrix4x4Test, CreateTranslation)
{
    Lateralus::Core::Vector3 translation(2.0f, 3.0f, 4.0f);
    Lateralus::Core::Matrix4x4 expectedTranslationMatrix(1.0f, 0.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f,
                                                         3.0f, 0.0f, 0.0f, 1.0f, 4.0f, 0.0f, 0.0f,
                                                         0.0f, 1.0f);

    Lateralus::Core::Matrix4x4 translationMatrix =
        Lateralus::Core::Matrix4x4::CreateTranslation(translation);

    ASSERT_NEAR(translationMatrix.r0.x, expectedTranslationMatrix.r0.x, 1e-5);
    ASSERT_NEAR(translationMatrix.r0.y, expectedTranslationMatrix.r0.y, 1e-5);
    ASSERT_NEAR(translationMatrix.r0.z, expectedTranslationMatrix.r0.z, 1e-5);
    ASSERT_NEAR(translationMatrix.r0.w, expectedTranslationMatrix.r0.w, 1e-5);

    ASSERT_NEAR(translationMatrix.r1.x, expectedTranslationMatrix.r1.x, 1e-5);
    ASSERT_NEAR(translationMatrix.r1.y, expectedTranslationMatrix.r1.y, 1e-5);
    ASSERT_NEAR(translationMatrix.r1.z, expectedTranslationMatrix.r1.z, 1e-5);
    ASSERT_NEAR(translationMatrix.r1.w, expectedTranslationMatrix.r1.w, 1e-5);

    ASSERT_NEAR(translationMatrix.r2.x, expectedTranslationMatrix.r2.x, 1e-5);
    ASSERT_NEAR(translationMatrix.r2.y, expectedTranslationMatrix.r2.y, 1e-5);
    ASSERT_NEAR(translationMatrix.r2.z, expectedTranslationMatrix.r2.z, 1e-5);
    ASSERT_NEAR(translationMatrix.r2.w, expectedTranslationMatrix.r2.w, 1e-5);

    ASSERT_NEAR(translationMatrix.r3.x, expectedTranslationMatrix.r3.x, 1e-5);
    ASSERT_NEAR(translationMatrix.r3.y, expectedTranslationMatrix.r3.y, 1e-5);
    ASSERT_NEAR(translationMatrix.r3.z, expectedTranslationMatrix.r3.z, 1e-5);
    ASSERT_NEAR(translationMatrix.r3.w, expectedTranslationMatrix.r3.w, 1e-5);
}

TEST(Matrix4x4Test, CreateRotation)
{
    Lateralus::Core::Vector3 axis(0.0f, 1.0f, 0.0f);
    float angle = 45.0f;

    float radians = angle * Lateralus::Core::Deg2Rad;
    float cosTheta = std::cos(radians);
    float sinTheta = std::sin(radians);

    Lateralus::Core::Matrix4x4 expectedRotationMatrix(cosTheta, 0.0f, sinTheta, 0.0f, 0.0f, 1.0f,
                                                      0.0f, 0.0f, -sinTheta, 0.0f, cosTheta, 0.0f,
                                                      0.0f, 0.0f, 0.0f, 1.0f);

    Lateralus::Core::Matrix4x4 rotationMatrix =
        Lateralus::Core::Matrix4x4::CreateRotation(axis, angle);

    ASSERT_NEAR(rotationMatrix.r0.x, expectedRotationMatrix.r0.x, 1e-5);
    ASSERT_NEAR(rotationMatrix.r0.y, expectedRotationMatrix.r0.y, 1e-5);
    ASSERT_NEAR(rotationMatrix.r0.z, expectedRotationMatrix.r0.z, 1e-5);
    ASSERT_NEAR(rotationMatrix.r0.w, expectedRotationMatrix.r0.w, 1e-5);

    ASSERT_NEAR(rotationMatrix.r1.x, expectedRotationMatrix.r1.x, 1e-5);
    ASSERT_NEAR(rotationMatrix.r1.y, expectedRotationMatrix.r1.y, 1e-5);
    ASSERT_NEAR(rotationMatrix.r1.z, expectedRotationMatrix.r1.z, 1e-5);
    ASSERT_NEAR(rotationMatrix.r1.w, expectedRotationMatrix.r1.w, 1e-5);

    ASSERT_NEAR(rotationMatrix.r2.x, expectedRotationMatrix.r2.x, 1e-5);
    ASSERT_NEAR(rotationMatrix.r2.y, expectedRotationMatrix.r2.y, 1e-5);
    ASSERT_NEAR(rotationMatrix.r2.z, expectedRotationMatrix.r2.z, 1e-5);
    ASSERT_NEAR(rotationMatrix.r2.w, expectedRotationMatrix.r2.w, 1e-5);

    ASSERT_NEAR(rotationMatrix.r3.x, expectedRotationMatrix.r3.x, 1e-5);
    ASSERT_NEAR(rotationMatrix.r3.y, expectedRotationMatrix.r3.y, 1e-5);
    ASSERT_NEAR(rotationMatrix.r3.z, expectedRotationMatrix.r3.z, 1e-5);
    ASSERT_NEAR(rotationMatrix.r3.w, expectedRotationMatrix.r3.w, 1e-5);
}

TEST(Matrix4x4Test, CreateRotationWithEuler)
{
    Lateralus::Core::Vector3 rotation(45.0f, 30.0f, 60.0f);

    float radX = rotation.x * Lateralus::Core::Deg2Rad;
    float radY = rotation.y * Lateralus::Core::Deg2Rad;
    float radZ = rotation.z * Lateralus::Core::Deg2Rad;

    float cosX = std::cos(radX);
    float sinX = std::sin(radX);
    float cosY = std::cos(radY);
    float sinY = std::sin(radY);
    float cosZ = std::cos(radZ);
    float sinZ = std::sin(radZ);

    Lateralus::Core::Matrix4x4 expectedRotationMatrix(
        cosY * cosZ, -cosX * sinZ + sinX * sinY * cosZ, sinX * sinZ + cosX * sinY * cosZ, 0.0f,
        cosY * sinZ, cosX * cosZ + sinX * sinY * sinZ, -sinX * cosZ + cosX * sinY * sinZ, 0.0f,
        -sinY, sinX * cosY, cosX * cosY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    Lateralus::Core::Matrix4x4 rotationMatrix =
        Lateralus::Core::Matrix4x4::CreateRotation(rotation);

    ASSERT_NEAR(rotationMatrix.r0.x, expectedRotationMatrix.r0.x, 1e-5);
    ASSERT_NEAR(rotationMatrix.r0.y, expectedRotationMatrix.r0.y, 1e-5);
    ASSERT_NEAR(rotationMatrix.r0.z, expectedRotationMatrix.r0.z, 1e-5);
    ASSERT_NEAR(rotationMatrix.r0.w, expectedRotationMatrix.r0.w, 1e-5);

    ASSERT_NEAR(rotationMatrix.r1.x, expectedRotationMatrix.r1.x, 1e-5);
    ASSERT_NEAR(rotationMatrix.r1.y, expectedRotationMatrix.r1.y, 1e-5);
    ASSERT_NEAR(rotationMatrix.r1.z, expectedRotationMatrix.r1.z, 1e-5);
    ASSERT_NEAR(rotationMatrix.r1.w, expectedRotationMatrix.r1.w, 1e-5);

    ASSERT_NEAR(rotationMatrix.r2.x, expectedRotationMatrix.r2.x, 1e-5);
    ASSERT_NEAR(rotationMatrix.r2.y, expectedRotationMatrix.r2.y, 1e-5);
    ASSERT_NEAR(rotationMatrix.r2.z, expectedRotationMatrix.r2.z, 1e-5);
    ASSERT_NEAR(rotationMatrix.r2.w, expectedRotationMatrix.r2.w, 1e-5);

    ASSERT_NEAR(rotationMatrix.r3.x, expectedRotationMatrix.r3.x, 1e-5);
    ASSERT_NEAR(rotationMatrix.r3.y, expectedRotationMatrix.r3.y, 1e-5);
    ASSERT_NEAR(rotationMatrix.r3.z, expectedRotationMatrix.r3.z, 1e-5);
    ASSERT_NEAR(rotationMatrix.r3.w, expectedRotationMatrix.r3.w, 1e-5);
}

TEST(Matrix4x4Test, CreateScale)
{
    Lateralus::Core::Vector3 scale(2.0f, 3.0f, 4.0f);

    Lateralus::Core::Matrix4x4 expectedScaleMatrix(2.0f, 0.0f, 0.0f, 0.0f, 
                                                    0.0f, 3.0f, 0.0f, 0.0f,
                                                   0.0f, 0.0f, 4.0f, 0.0f, 
                                                    0.0f, 0.0f, 0.0f, 1.0f);

    Lateralus::Core::Matrix4x4 scaleMatrix = Lateralus::Core::Matrix4x4::CreateScale(scale);

    ASSERT_NEAR(scaleMatrix.r0.x, expectedScaleMatrix.r0.x, 1e-5);
    ASSERT_NEAR(scaleMatrix.r0.y, expectedScaleMatrix.r0.y, 1e-5);
    ASSERT_NEAR(scaleMatrix.r0.z, expectedScaleMatrix.r0.z, 1e-5);
    ASSERT_NEAR(scaleMatrix.r0.w, expectedScaleMatrix.r0.w, 1e-5);

    ASSERT_NEAR(scaleMatrix.r1.x, expectedScaleMatrix.r1.x, 1e-5);
    ASSERT_NEAR(scaleMatrix.r1.y, expectedScaleMatrix.r1.y, 1e-5);
    ASSERT_NEAR(scaleMatrix.r1.z, expectedScaleMatrix.r1.z, 1e-5);
    ASSERT_NEAR(scaleMatrix.r1.w, expectedScaleMatrix.r1.w, 1e-5);

    ASSERT_NEAR(scaleMatrix.r2.x, expectedScaleMatrix.r2.x, 1e-5);
    ASSERT_NEAR(scaleMatrix.r2.y, expectedScaleMatrix.r2.y, 1e-5);
    ASSERT_NEAR(scaleMatrix.r2.z, expectedScaleMatrix.r2.z, 1e-5);
    ASSERT_NEAR(scaleMatrix.r2.w, expectedScaleMatrix.r2.w, 1e-5);

    ASSERT_NEAR(scaleMatrix.r3.x, expectedScaleMatrix.r3.x, 1e-5);
    ASSERT_NEAR(scaleMatrix.r3.y, expectedScaleMatrix.r3.y, 1e-5);
    ASSERT_NEAR(scaleMatrix.r3.z, expectedScaleMatrix.r3.z, 1e-5);
    ASSERT_NEAR(scaleMatrix.r3.w, expectedScaleMatrix.r3.w, 1e-5);
}

TEST(Matrix4x4Test, CreateOrthographicProjection)
{
    float left = -2.0f;
    float right = 2.0f;
    float bottom = -1.0f;
    float top = 1.0f;
    float near = 3.0f;
    float far = 7.0f;

    Lateralus::Core::Matrix4x4 expectedProjectionMatrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, -2.5f,
        0.0f, 0.0f, 0.0f, 1.0f);

    Lateralus::Core::Matrix4x4 projectionMatrix =
        Lateralus::Core::Matrix4x4::CreateOrthographicProjection(left, right, bottom, top, near,
                                                                 far);

    ASSERT_NEAR(projectionMatrix.r0.x, expectedProjectionMatrix.r0.x, 1e-5);
    ASSERT_NEAR(projectionMatrix.r0.y, expectedProjectionMatrix.r0.y, 1e-5);
    ASSERT_NEAR(projectionMatrix.r0.z, expectedProjectionMatrix.r0.z, 1e-5);
    ASSERT_NEAR(projectionMatrix.r0.w, expectedProjectionMatrix.r0.w, 1e-5);

    ASSERT_NEAR(projectionMatrix.r1.x, expectedProjectionMatrix.r1.x, 1e-5);
    ASSERT_NEAR(projectionMatrix.r1.y, expectedProjectionMatrix.r1.y, 1e-5);
    ASSERT_NEAR(projectionMatrix.r1.z, expectedProjectionMatrix.r1.z, 1e-5);
    ASSERT_NEAR(projectionMatrix.r1.w, expectedProjectionMatrix.r1.w, 1e-5);

    ASSERT_NEAR(projectionMatrix.r2.x, expectedProjectionMatrix.r2.x, 1e-5);
    ASSERT_NEAR(projectionMatrix.r2.y, expectedProjectionMatrix.r2.y, 1e-5);
    ASSERT_NEAR(projectionMatrix.r2.z, expectedProjectionMatrix.r2.z, 1e-5);
    ASSERT_NEAR(projectionMatrix.r2.w, expectedProjectionMatrix.r2.w, 1e-5);

    ASSERT_NEAR(projectionMatrix.r3.x, expectedProjectionMatrix.r3.x, 1e-5);
    ASSERT_NEAR(projectionMatrix.r3.y, expectedProjectionMatrix.r3.y, 1e-5);
    ASSERT_NEAR(projectionMatrix.r3.z, expectedProjectionMatrix.r3.z, 1e-5);
    ASSERT_NEAR(projectionMatrix.r3.w, expectedProjectionMatrix.r3.w, 1e-5);
}

TEST(Matrix4x4Test, CreatePerspectiveProjection)
{
    float fov = 60.0f;
    float aspectRatio = 16.0f / 9.0f;
    float near = 0.1f;
    float far = 100.0f;

    float tanHalfFov = std::tan(0.5f * fov * Lateralus::Core::Deg2Rad);
    float zRange = far - near;

    Lateralus::Core::Matrix4x4 expectedProjectionMatrix(
        1.0f / (aspectRatio * tanHalfFov), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, -(far + near) / zRange, -2.0f * far * near / zRange, 0.0f, 0.0f, -1.0f, 0.0f);

    Lateralus::Core::Matrix4x4 projectionMatrix =
        Lateralus::Core::Matrix4x4::CreatePerspectiveProjection(fov, aspectRatio, near, far);

    ASSERT_NEAR(projectionMatrix.r0.x, expectedProjectionMatrix.r0.x, 1e-5);
    ASSERT_NEAR(projectionMatrix.r0.y, expectedProjectionMatrix.r0.y, 1e-5);
    ASSERT_NEAR(projectionMatrix.r0.z, expectedProjectionMatrix.r0.z, 1e-5);
    ASSERT_NEAR(projectionMatrix.r0.w, expectedProjectionMatrix.r0.w, 1e-5);

    ASSERT_NEAR(projectionMatrix.r1.x, expectedProjectionMatrix.r1.x, 1e-5);
    ASSERT_NEAR(projectionMatrix.r1.y, expectedProjectionMatrix.r1.y, 1e-5);
    ASSERT_NEAR(projectionMatrix.r1.z, expectedProjectionMatrix.r1.z, 1e-5);
    ASSERT_NEAR(projectionMatrix.r1.w, expectedProjectionMatrix.r1.w, 1e-5);

    ASSERT_NEAR(projectionMatrix.r2.x, expectedProjectionMatrix.r2.x, 1e-5);
    ASSERT_NEAR(projectionMatrix.r2.y, expectedProjectionMatrix.r2.y, 1e-5);
    ASSERT_NEAR(projectionMatrix.r2.z, expectedProjectionMatrix.r2.z, 1e-5);
    ASSERT_NEAR(projectionMatrix.r2.w, expectedProjectionMatrix.r2.w, 1e-5);

    ASSERT_NEAR(projectionMatrix.r3.x, expectedProjectionMatrix.r3.x, 1e-5);
    ASSERT_NEAR(projectionMatrix.r3.y, expectedProjectionMatrix.r3.y, 1e-5);
    ASSERT_NEAR(projectionMatrix.r3.z, expectedProjectionMatrix.r3.z, 1e-5);
    ASSERT_NEAR(projectionMatrix.r3.w, expectedProjectionMatrix.r3.w, 1e-5);
}

TEST(Matrix4x4Test, CreateLookAt)
{
    Lateralus::Core::Vector3 eye(0.0f, 0.0f, 3.0f);
    Lateralus::Core::Vector3 target(0.0f, 0.0f, 0.0f);
    Lateralus::Core::Vector3 up(0.0f, 1.0f, 0.0f);

    Lateralus::Core::Vector3 zAxis = (eye - target).Normalized();
    Lateralus::Core::Vector3 xAxis = up.Cross(zAxis).Normalized();
    Lateralus::Core::Vector3 yAxis = zAxis.Cross(xAxis);

    Lateralus::Core::Matrix4x4 expectedLookAtMatrix(
        xAxis.x, xAxis.y, xAxis.z, -xAxis.Dot(eye),
        yAxis.x, yAxis.y, yAxis.z, -yAxis.Dot(eye),
        -zAxis.x, -zAxis.y, -zAxis.z, zAxis.Dot(eye),
        0.0f, 0.0f, 0.0f, 1.0f);

    Lateralus::Core::Matrix4x4 lookAtMatrix =
        Lateralus::Core::Matrix4x4::CreateLookAt(eye, target, up);

    ASSERT_NEAR(lookAtMatrix.r0.x, expectedLookAtMatrix.r0.x, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r0.y, expectedLookAtMatrix.r0.y, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r0.z, expectedLookAtMatrix.r0.z, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r0.w, expectedLookAtMatrix.r0.w, 1e-5);

    ASSERT_NEAR(lookAtMatrix.r1.x, expectedLookAtMatrix.r1.x, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r1.y, expectedLookAtMatrix.r1.y, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r1.z, expectedLookAtMatrix.r1.z, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r1.w, expectedLookAtMatrix.r1.w, 1e-5);

    ASSERT_NEAR(lookAtMatrix.r2.x, expectedLookAtMatrix.r2.x, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r2.y, expectedLookAtMatrix.r2.y, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r2.z, expectedLookAtMatrix.r2.z, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r2.w, expectedLookAtMatrix.r2.w, 1e-5);

    ASSERT_NEAR(lookAtMatrix.r3.x, expectedLookAtMatrix.r3.x, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r3.y, expectedLookAtMatrix.r3.y, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r3.z, expectedLookAtMatrix.r3.z, 1e-5);
    ASSERT_NEAR(lookAtMatrix.r3.w, expectedLookAtMatrix.r3.w, 1e-5);
}

} // namespace Lateralus::Core::Tests