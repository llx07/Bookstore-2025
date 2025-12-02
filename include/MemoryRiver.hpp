#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <queue>

template <class T, int info_len = 2>
class MemoryRiver {
private:
    std::queue<int> q;
    int count = 0;
    std::fstream file;
    std::string file_name;
    static constexpr int sizeofT = sizeof(T);

    void open_file(){
        if (file_name.empty()) ;
    }

public:
    MemoryRiver() = default;

    MemoryRiver(const std::string& _file_name) : file_name(_file_name) {}

    void initialise(std::string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out | std::ios::binary);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i) file.write(reinterpret_cast<char*>(&tmp), sizeof(int));
        file.close();
        // std::cerr << "FILE CREATE";
    }

    // 读出第n个int的值赋给tmp，1_base
    void get_info(int& tmp, int n) {
        if (n > info_len) return;
        file.open(file_name, std::ios::in | std::ios::binary);
        file.seekg((n - 1) * sizeof(int));
        file.read(reinterpret_cast<char*>(&tmp), sizeof(int));
        file.close();
    }

    // 将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp((n - 1) * sizeof(int));
        file.write(reinterpret_cast<const char*>(&tmp), sizeof(int));
        file.close();
    }

    // 在文件合适位置写入类对象t，并返回写入的位置索引index
    // 位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    // 位置索引index可以取为对象写入的起始位置
    int write(const T& t) {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if(q.empty()){
            file.seekp((info_len) * sizeof(int) + sizeofT * count);
            file.write(reinterpret_cast<const char*>(&t), sizeofT);
            file.close();
            return ++count;
        }
        else{
            int pos = q.front(); q.pop();
            file.seekp((info_len) * sizeof(int) + sizeofT * (pos-1));
            file.write(reinterpret_cast<const char*>(&t), sizeofT);
            file.close();
            return pos;
        }
    }

    // 用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(const T& t, const int index) {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp((info_len) * sizeof(int) + sizeofT * (index - 1));
        file.write(reinterpret_cast<const char*>(&t), sizeofT);
        file.close();
    }

    // 读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T& t, const int index) {
        file.open(file_name, std::ios::in | std::ios::binary);
        file.seekg((info_len) * sizeof(int) + sizeofT * (index - 1));
        file.read(reinterpret_cast<char*>(&t), sizeofT);
        file.close();
    }

    // 删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {
        q.push(index);
    }
};

#endif  // BPT_MEMORYRIVER_HPP