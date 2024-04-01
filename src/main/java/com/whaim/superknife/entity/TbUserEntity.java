package com.whaim.superknife.entity;

import jakarta.persistence.*;

@Entity
@Table(name = "TB_USER", schema = "PUBLIC", catalog = "DB_TEST")
public class TbUserEntity {
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Id
    @Column(name = "USER_ID")
    private int userId;
    @Basic
    @Column(name = "USER_NAME")
    private Object userName;

    public int getUserId() {
        return userId;
    }

    public void setUserId(int userId) {
        this.userId = userId;
    }

    public Object getUserName() {
        return userName;
    }

    public void setUserName(Object userName) {
        this.userName = userName;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        TbUserEntity that = (TbUserEntity) o;

        if (userId != that.userId) return false;
        if (userName != null ? !userName.equals(that.userName) : that.userName != null) return false;

        return true;
    }

    @Override
    public int hashCode() {
        int result = userId;
        result = 31 * result + (userName != null ? userName.hashCode() : 0);
        return result;
    }
}
