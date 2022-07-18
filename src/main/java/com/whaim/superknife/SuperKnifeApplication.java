package com.whaim.superknife;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.jms.annotation.EnableJms;

@EnableJms
@SpringBootApplication
public class SuperKnifeApplication {

    public static void main(String[] args) {
        SpringApplication.run(SuperKnifeApplication.class, args);
    }

}
