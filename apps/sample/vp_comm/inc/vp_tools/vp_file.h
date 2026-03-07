//
// Created by 杨柳 on 2022/11/25.
//

#ifndef VP_FRAMEWORK_VP_FILE_H
#define VP_FRAMEWORK_VP_FILE_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ACCESSPERMS
# define ACCESSPERMS (S_IRWXU|S_IRWXG|S_IRWXO) /* 0777 */
#endif
#ifndef ALLPERMS
# define ALLPERMS (S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)/* 07777 */
#endif
#ifndef DEFFILEMODE
# define DEFFILEMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)/* 0666*/
#endif

/**
 * 获取路径目录
 *
 * @param path  [in]    全路径
 * @param dir   [out]   输出目录地址
 *
 * @return 目录长度
 */
size_t vp_dirname(const char *path, const char **dir);

/**
 * 获取路径目录,拷贝进[dir]
 *
 * @param path  [in]    全路径
 * @param dir   [in]    目录存储地址
 * @param len   [in]    存储长度
 *
 * @return 目录长度
 */
size_t vp_dirname_r(const char *path, char *dir, size_t len);

/**
 * 获取路径文件名
 *
 * @param path      [in]    全路径
 * @param filename  [out]   输出文件名地址
 *
 * @return 文件名长度
 */
size_t vp_filename(const char *path, const char **filename);

/**
 * 获取路径文件名,拷贝进[filename]
 *
 * @param path      [in]    全路径
 * @param filename  [in]    文件名存储地址
 * @param len       [in]    存储长度
 *
 * @return 文件名长度
 */
size_t vp_filename_r(const char *path, char *filename, size_t len);

/**
 * 创建全目录路径
 *
 * @param path      [in]    全路径
 * @param len       [in]    路径长度
 * @param perms     [in]    权限,ALL
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_mkdir(const char *path, size_t len, int perms);

/**
 * 删除目录
 *
 * @param path
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_del_dir(const char *path);

/**
 * 拼合路径
 *
 * @param path  [in]    路径存储地址
 * @param size  [in]    可存储长度
 * @param ...   [in]    多个字符串,NULL结束
 *
 * @return 拼合路径长度
 */
size_t vp_path_join(char *path, size_t size, ...);

/**
 * 文件拷贝
 *
 * @param src   [in]    源文件全路径
 * @param dst   [in]    目标文件全路径
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_file_copy(const char *src, const char *dst);

/**
 * 文件填充
 *
 * @param path  [in]    文件路径
 * @param size  [in]   填充大小
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_file_filling(const char *path, int size);

/**
 * 文件填充
 *
 * @param file  [in]   文件
 * @param size  [in]   填充大小
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_file_ffilling(FILE *file, int size);

/**
 * 文件隐藏
 *
 * @param path  [in]    文件全路径
 * @param system [in]   是否标记为系统保护文件
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_file_hide(const char *path, int system);

/**
 * 文件隐藏
 *
 * @param ifd  [in]    文件描述符
 * @param system [in]  是否标记为系统保护文件
 *
 * @return 错误码
 * @retval =0 成功
 * @retval !0 失败
 */
int vp_fd_hide(int fd, int system);

/**
 * 创建指定数据的文件或指定长度的空数据文件
 * @note 如果文件已存在,将会覆盖文件
 *
 * @param path  [in]    文件全路径
 * @param data  [in]    需要写入的数据,如仅需写入长度,则传入NULL
 *                      该值不为空将创建文件并写入数据,最后会关闭文件.
 *                      返回写入数据的长度
 * @param len   [in]    写入数据长度或需要分配的文件长度
 *
 * @return 错误码或成功写入数据长度
 * @retval >=0 创建成功,成功写入数据长度
 * @retval  <0 创建失败
 */
ssize_t vp_file_create(const char *path, void *data, size_t len);

/**
 * 向文件中循环写入数据,直到发生错误
 *
 * @note 该方法是针对`write`进行的封装,主要用于循环写入数据
 * 如果`write`返回0或者-1该函数将退出循环写入并返回已成功写入的数据
 * 当前函数返回值与参数`len`不相等时则表示写入发生了错误
 *
 * @param fd    [in]    文件描述符
 * @param buf   [in]    数据缓冲区,指向需要写入的数据
 * @param len   [in]    写入数据长度
 * @return
 * @retval >=0 成功写入数据的长度
 * @retval  <0 写入失败
 */
ssize_t vp_file_write(int fd, const void *buf, size_t len);

/**
 * 从文件中循环读取数据,直到发送错误,或读取到文件结尾
 *
 * @note 该方法是针对`read`进行的封装,主要用于循环读取数据
 * 如果`read`返回0或者-1该函数将退出循环读取并返回已成功读取的数据
 * 当前函数返回值与参数`len`不相等时则表示读取发送了错误,或读取到文件结尾
 *
 * @param fd    [in]    文件描述符
 * @param buf   [in]    数据缓冲区,指向接收数据的缓冲区
 * @param len   [in]    读取数据长度
 * @return
 * @retval >=0 成功读取数据的长度
 * @retval  <0 写入失败
 */
ssize_t vp_file_read(int fd, void *buf, size_t len);

/**
 * 先文件中循环写入数据,直到发送错误
 *
 * @note 该方法是针对`write`进行的封装,主要用于循环写入数据
 * 如果`write`返回0或者-1该函数将退出循环写入并返回已成功写入的数据
 * 当前函数返回值与参数`len`不相等时则表示写入发送了错误
 *
 * @param file    [in]  文件描述符
 * @param buf   [in]    数据缓冲区,指向需要写入的数据
 * @param len   [in]    写入数据长度
 * @return
 * @retval >=0 成功写入数据的长度
 * @retval  <0 写入失败
 */
ssize_t vp_file_fwrite(FILE *file, const void *buf, size_t len);

/**
 * 修改文件句柄对应文件的名称
 *
 * @param fd           [in]    文件描述符
 * @param file_path    [in]    文件路径
 *
 * @return
 * @retval  <0 操作失败
 */
int vp_file_rename(int fd, const char *file_path);

/**
 * 完成文件操作,刷新缓存,关闭文件
 *
 * @note 该方法按顺序调用函数`fdatasync`,`fsync`,`close`
 *
 * @param fd    [in]    文件描述符
 *
 * @return
 * @retval >=0 当前文件描述符偏移量`lseek(fd,0,SEEK_CUR)`<br></br>
 *             对于写操作,该值可以表示为当前文件已写入量
 *             对于读操作,该值可以表示为文件已被读取数量
 * @retval  <0 操作失败
 */
ssize_t vp_file_done(int fd);


/**
 * 删除并关闭文件
 *
 * @note 删除文件会先将当前文件截断长度为0,然后将文件修改为'/tmp/del.tmp',最后将删除'/tmp/del.tmp'
 *
 * @param fd    [in]    文件描述符
 *
 * @return
 * @retval =0 成功
 * @retval <0 失败
 */
int vp_file_del(int fd);

/**
 * 获取文件长度
 *
 * @param fd    [in]    文件描述符
 *
 * @return
 */
off_t vp_file_len(int fd);

#ifdef __cplusplus
}
#endif
#endif //VP_FRAMEWORK_VP_FILE_H
