1. 系统概述
图书馆管理系统是一个基于C语言开发的控制台应用程序，用于管理图书馆的图书信息、用户信息、借阅记录以及逾期罚款计算。系统采用模块化设计，使用链表数据结构来动态管理数据，并通过文件存储实现数据的持久化。

2. 程序结构
图书馆管理系统
├── 注册/登录
│   ├── create_user（注册功能）
│   └── login（登录功能）
│       ├── checkUserValid（用户验证）
│       └── check_usernum（用户名校验）
├── 读者功能
│   ├── reader_search_books（查询书籍）
│   │   ├── search_by_number（按编号查询）
│   │   ├── search_by_name（按书名查询）
│   │   └── search_by_category（按分类查询）
│   ├── enhanced_borrow_book（借书）
│   │   ├── 图书查找（search_by_number/name）
│   │   ├── 状态检查（检查lent字段）
│   │   └── append_borrow_record（记录添加）
│   ├── 还书功能（main函数中实现）
│   │   ├── search_by_number（图书查找）
│   │   ├── 状态更新（修改lent字段）
│   │   └── mark_record_returned（记录标记）
│   ├── print_borrow_rules（借阅规则）
│   └── list_borrow_records_by_user（借阅记录）
├── 系统管理员功能
│   ├── delete_sysuser（删除用户）
│   │   ├── search_by_usernum（用户查找）
│   │   └── override_to_sysuser_file（文件更新）
│   ├── print_all_sysuser（用户列表）
│   │   ├── make_sysuser_linklist（链表创建）
│   │   └── print_sysusernode（数据显示）
│   ├── list_all_borrow_records（全部借阅记录）
│   │   ├── make_borrow_linklist（记录链表）
│   │   └── print_one_borrow（记录显示）
│   └── list_borrow_records_by_user（用户借阅查询）
├── 图书管理员功能
│   ├── input_new_book（录入图书）
│   │   └── 文件追加（直接文件操作）
│   ├── modify_book（修改图书）
│   │   ├── search_by_number（图书查找）
│   │   └── override_to_file（文件更新）
│   └── delete_book（删除图书）
│       ├── search_by_number（图书查找）
│       └── override_to_file（文件更新）
├── 核心支撑函数
│   ├── make_linklist（图书链表创建）
│   ├── days_between_dates（日期计算）
│   ├── calc_overdue_fine（罚款计算）
│   ├── override_to_file（图书文件更新）
│   └── override_borrow_file（借阅记录更新）
├── 显示函数
│   ├── print_main_title（主标题）
│   ├── print_booklist_title（图书列表标题）
│   ├── print_booknode（单本图书信息）
│   ├── print_sysuserlist_title（用户列表标题）
│   ├── print_sysusernode（单个用户信息）
│   └── print_borrow_title（借阅记录标题）
└── 文件检查函数
    ├── check_void_file（图书文件检查）
    ├── check_sysuser_void_file（用户文件检查）
    └── check_borrow_void_file（借阅文件检查）

3.核心算法
1. 链表管理算法
线性搜索：遍历链表查找图书/用户，时间复杂度O(n)
动态内存管理：使用malloc/free动态管理节点内存
文件数据同步：链表与文本文件实时同步，保证数据持久化
2. 日期计算算法
时间戳转换：使用mktime将日期转为时间戳计算差值
逾期自动计算：系统自动计算借阅天数，超期按天计费
实时更新：基于系统当前时间进行动态计算
3. 数据持久化算法
全量覆盖写入：修改数据时重写整个文件确保一致性
增量追加记录：借阅记录采用追加模式提高性能
文本格式存储：使用结构化文本便于阅读和维护
4. 用户认证算法
线性匹配验证：遍历用户文件进行账号密码匹配
多角色权限控制：不同角色拥有不同的功能访问权限
输入安全处理：密码输入时隐藏字符显示这些名称符合我的代码吗