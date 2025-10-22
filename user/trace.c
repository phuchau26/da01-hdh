// user/trace.c
#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];
  char path[64]; // Bộ đệm tạm thời cho đường dẫn tuyệt đối

  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command [args...]\n", argv[0]);
    exit(1);
  }

  // 1. Gọi syscall trace
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }
  
  // 2. Chuẩn bị đối số nargv
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }
  
  // Kiểm tra giới hạn MAXARG
  if(argc - 2 >= MAXARG) {
    fprintf(2, "%s: Too many arguments\n", argv[0]);
    exit(1);
  }
  
  // Đánh dấu kết thúc mảng đối số
  nargv[argc-2] = 0; 

  // 3. Sửa lỗi EXEC: Đảm bảo đường dẫn tuyệt đối
  
  // Lấy tên chương trình con (argv[2])
  char *cmd = argv[2]; 

  // Nếu tên chưa bắt đầu bằng '/', thêm '/'
  if (cmd[0] != '/') {
      path[0] = '/';
      uint len = strlen(cmd);
      for(uint j = 0; j <= len && j < 63; j++) {
          path[j+1] = cmd[j];
      }
      cmd = path; // Đặt cmd là đường dẫn tuyệt đối mới
  }

  // 4. Gọi exec
  exec(cmd, nargv);

  // 5. Nếu exec thất bại
  fprintf(2, "trace: exec %s failed\n", cmd);
  exit(1); // Trả về mã lỗi
}