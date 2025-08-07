-- 创建用户表
CREATE TABLE userinfo (
    userid TEXT PRIMARY KEY NOT NULL,
    passwd TEXT NOT NULL,  -- 存储加盐哈希后的密码
    salt TEXT NOT NULL,           -- 密码盐值
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 创建系统参数表
create table sysparam (
    paramcode TEXT PRIMARY KEY NOT NULL,
    paramvalue TEXT NOT NULL,
    paramdesc TEXT,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建角色表
create table role (
    roleid TEXT PRIMARY KEY NOT NULL,
    rolename TEXT NOT NULL,
    roledesc TEXT,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建用户角色关联表
create table userrole (
    userid TEXT NOT NULL,
    roleid TEXT NOT NULL,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (userid, roleid)
);
-- 创建权限表
create table permission (
    permissionid TEXT PRIMARY KEY NOT NULL,
    permissionname TEXT NOT NULL,
    permissiondesc TEXT,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建角色权限关联表
create table rolepermission (
    roleid TEXT NOT NULL,
    permissionid TEXT NOT NULL,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (roleid, permissionid)
);
-- 创建菜单表
create table menu (
    menuid TEXT PRIMARY KEY NOT NULL,
    parentid TEXT,
    title TEXT NOT NULL,
    pathurl TEXT,
    component TEXT,
    icon TEXT,
    sortorder INTEGER DEFAULT 0,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建角色菜单关联表
create table rolemenu (
    roleid TEXT NOT NULL,
    menuid TEXT NOT NULL,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (roleid, menuid)
);
-- 创建操作日志表
create table operationlog (
    logid INTEGER PRIMARY KEY AUTOINCREMENT,
    userid TEXT NOT NULL,
    operation TEXT NOT NULL,
    operationtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    details TEXT
);
-- 创建数据字典表
create table datadict (
    dictid TEXT PRIMARY KEY NOT NULL,
    dictname TEXT NOT NULL,
    dictvalue TEXT NOT NULL,
    dictdesc TEXT,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建文件存储表
create table filestorage (
    fileid TEXT PRIMARY KEY NOT NULL,
    filename TEXT NOT NULL,
    filepath TEXT NOT NULL,
    filesize INTEGER NOT NULL,
    filetype TEXT,
    uploadtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    uploader TEXT
);
-- 创建通知公告表
create table notification (
    notificationid TEXT PRIMARY KEY NOT NULL,
    title TEXT NOT NULL,
    content TEXT NOT NULL,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    author TEXT
);
-- 创建系统设置表
create table systemsettings (
    settingid TEXT PRIMARY KEY NOT NULL,
    settingkey TEXT NOT NULL,
    settingvalue TEXT NOT NULL,
    settingdesc TEXT,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建会话表
create table sessions (
    sessionid TEXT PRIMARY KEY NOT NULL,
    userid TEXT NOT NULL,
    createdat TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expiresat TIMESTAMP NOT NULL
);
-- 创建多因素认证表
create table mfa (
    mfaid TEXT PRIMARY KEY NOT NULL,
    userid TEXT NOT NULL,
    mfatype TEXT NOT NULL,
    secret TEXT NOT NULL,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建审计日志表
create table auditlog (
    auditid INTEGER PRIMARY KEY AUTOINCREMENT,
    userid TEXT NOT NULL,
    action TEXT NOT NULL,
    actiontime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    details TEXT
);
-- 创建API密钥表
create table apikeys (
    apikeyid TEXT PRIMARY KEY NOT NULL,
    userid TEXT NOT NULL,
    apikey TEXT NOT NULL,
    apikeydesc TEXT,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建通知订阅表
create table subscriptions (
    subscriptionid TEXT PRIMARY KEY NOT NULL,
    userid TEXT NOT NULL,
    subscriptiontype TEXT NOT NULL,
    target TEXT NOT NULL,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建系统健康检查表
create table systemhealth (
    checkid INTEGER PRIMARY KEY AUTOINCREMENT,
    checkname TEXT NOT NULL,
    status TEXT NOT NULL,
    lastchecked TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    details TEXT
);
-- 创建任务调度表
create table taskschedule (
    taskid TEXT PRIMARY KEY NOT NULL,
    taskname TEXT NOT NULL,
    schedule TEXT NOT NULL,
    lastrun TIMESTAMP,
    nextrun TIMESTAMP,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建缓存表
create table cache (
    cachekey TEXT PRIMARY KEY NOT NULL,
    cachevalue TEXT NOT NULL,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expiresat TIMESTAMP NOT NULL
);
-- 创建审计配置表
create table auditconfig (
    configid TEXT PRIMARY KEY NOT NULL,
    configname TEXT NOT NULL,
    configvalue TEXT NOT NULL,
    configdesc TEXT,
    createtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
-- 创建系统更新日志表
create table systemupdates (
    updateid INTEGER PRIMARY KEY AUTOINCREMENT,
    version TEXT NOT NULL,
    description TEXT,
    updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);  
-- 创建API访问日志表
create table apiaccesslog (
    logid INTEGER PRIMARY KEY AUTOINCREMENT,
    apikeyid TEXT NOT NULL,
    endpoint TEXT NOT NULL,
    accessime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    responsecode INTEGER,
    details TEXT
);
-- 创建错误日志表
create table errorlog (
    errorid INTEGER PRIMARY KEY AUTOINCREMENT,
    userid TEXT,
    errormessage TEXT NOT NULL,
    errortime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    stacktrace TEXT
);
-- 创建系统维护记录表
create table systemmaintenance (
    maintenanceid INTEGER PRIMARY KEY AUTOINCREMENT,
    maintenancedesc TEXT NOT NULL,
    maintenancetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    performedby TEXT
);
-- 创建系统统计表
create table systemstats (
    statid INTEGER PRIMARY KEY AUTOINCREMENT,
    statname TEXT NOT NULL,
    statvalue INTEGER NOT NULL,
    recordtime TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


