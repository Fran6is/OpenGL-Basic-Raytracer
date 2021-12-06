#include "GlmTransformsPrint.h"


std::ostream& operator<<(std::ostream& cout, const glm::vec3& vector)
{
    printf("< %3.2f, %3.2f, %3.2f >", vector.x, vector.y, vector.z);
    return cout;
}

std::ostream& operator<<(std::ostream& cout, const glm::vec4& vector)
{
    printf("< %3.2f, %3.2f, %3.2f, %3.2f >", vector.x, vector.y, vector.z, vector.w);

    return cout;
}

std::ostream& operator<<(std::ostream& cout, const glm::mat3& Matrix)
{
    std::cout << "========================================MATRIX==========================================\n";
    printf("   X|   Y|    Z|  \n");
    printf("%3.2f, %3.2f, %3.2f\n", Matrix[0][0], Matrix[1][0], Matrix[2][0] );
    printf("%3.2f, %3.2f, %3.2f\n", Matrix[0][1], Matrix[1][1], Matrix[2][1] );
    printf("%3.2f, %3.2f, %3.2f\n", Matrix[0][2], Matrix[1][2], Matrix[2][2] );
    cout << "-------------------------------\n";
    cout << "X = " << Matrix[0] << "\n";
    cout << "Y = " << Matrix[1] << "\n";
    cout << "Z = " << Matrix[2] << "\n";
    std::cout << "=======================================================================================\n";

    return cout;
}

std::ostream& operator<<(std::ostream& cout, const glm::mat4& Matrix)
{
    std::cout << "========================================MATRIX==========================================\n";
    printf("   X|   Y|    Z|     W|  \n");
    printf("%3.2f, %3.2f, %3.2f, %3.2f\n", Matrix[0][0], Matrix[1][0], Matrix[2][0], Matrix[3][0] );
    printf("%3.2f, %3.2f, %3.2f, %3.2f\n", Matrix[0][1], Matrix[1][1], Matrix[2][1], Matrix[3][1] );
    printf("%3.2f, %3.2f, %3.2f, %3.2f\n", Matrix[0][2], Matrix[1][2], Matrix[2][2], Matrix[3][2] );
    printf("%3.2f, %3.2f, %3.2f, %3.2f\n", Matrix[0][3], Matrix[1][3], Matrix[2][3], Matrix[3][3] );
    cout << "-------------------------------\n";
    cout << "X = " << Matrix[0] << "\n";
    cout << "Y = " << Matrix[1] << "\n";
    cout << "Z = " << Matrix[2] << "\n";
    cout << "W = " << Matrix[3] << "\n";
    std::cout << "=======================================================================================\n";


    return cout;
}

