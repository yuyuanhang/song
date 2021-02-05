#pragma once

#include"config.h"
#include"logger.h"
#include<stdlib.h>
#include<memory>
#include<vector>
#include<functional>
#include<fstream>



class Parser{
private:
	const int ONE_BASED_LIBSVM = 1;
    const int MAX_LINE = 10000000;
    std::function<void(idx_t,std::vector<std::pair<int,value_t>>)> consume;

    std::vector<int> tokenize(char* buff){
        std::vector<int> ret;
        int i = 0;
        while(*(buff + i) != '\0'){
            if(*(buff + i) == ':' || *(buff + i) == ' ')
                ret.push_back(i);
            ++i;
        }
        for(int i = 0; i < ret.size(); i++){
            printf("%d\n", ret[i]);
        }
        return ret;
    }

    std::vector<std::pair<int,value_t>> parse(std::vector<int> tokens,char* buff){
        std::vector<std::pair<int,value_t>> ret;
        ret.reserve(tokens.size() / 2);
        for(int i = 0;i + 1 < tokens.size();i += 2){
            int index;
            value_t val;
            sscanf(buff + tokens[i] + 1,"%d",&index);
			index -= ONE_BASED_LIBSVM;
			double tmp;
            sscanf(buff + tokens[i + 1] + 1,"%lf",&tmp);
			val = tmp;
            ret.push_back(std::make_pair(index,val));
            printf("%d %lf\n", index, val);
        }
        return ret;
    }


public:

    Parser(const char* path,std::function<void(idx_t,std::vector<std::pair<int,value_t>>)> consume) : consume(consume){
        /*auto fp = fopen(path,"r");
        if(fp == NULL){
            Logger::log(Logger::ERROR,"File not found at (%s)\n",path);
            exit(1);
        }
        std::unique_ptr<char[]> buff(new char[MAX_LINE]);
        std::vector<std::string> buffers;*/
        std::ifstream in(path, std::ios::binary);
        unsigned dim;

        if (!in.is_open()) {
            Logger::log(Logger::ERROR,"File not found at (%s)\n",path);
            exit(1);
        }
        in.read((char*)&dim, 4);
        in.seekg(0, std::ios::end); 
        std::ios::pos_type ss = in.tellg(); 
        size_t fsize = (size_t)ss;
        unsigned num = (unsigned)(fsize / (dim + 1) / 4);    
        float* data = new float[(size_t)dim];
        printf("%d %d\n", dim, num);

        idx_t idx = 0;
    
        in.seekg(0, std::ios::beg); 
        for (size_t i = 0; i < num; i++) {
            in.seekg(4, std::ios::cur); 
            in.read((char*)(data), dim * 4);  
            std::vector<std::pair<int,value_t>> values;
            for(int j = 0; j < dim; j++){
                values.push_back(std::make_pair(j, data[j]));
            }
            if(i < 2){
                for(int j = 0; j < dim; j++){
                    printf("(%d %f) ", values[j].first, values[j].second);
                }
                printf("\n");
            }
            consume(idx,values);
            ++idx;
            if(i % 1000 == 0){
                printf("%d\n", i);
            }
        }
        in.close();

        /*while(fgets(buff.get(),MAX_LINE,fp)){
            if(idx == 0){
                printf("%s\n", buff.get());
            }
            auto tokens = tokenize(buff.get());
            auto values = parse(tokens,buff.get());
            consume(idx,values);
            ++idx;
        }
        fclose(fp);*/
    }
    
};
