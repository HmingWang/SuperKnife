package com.whaim.superknife.configuration;

import com.ibm.mq.jms.MQConnectionFactory;
import com.ibm.mq.spring.boot.MQConfigurationProperties;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;
import org.springframework.context.annotation.Scope;
import org.springframework.jms.core.JmsTemplate;


@Configuration
public class JmsConfig {
    @Primary
    @Bean
    MQConfigurationProperties mqConfigurationProperties() {
        MQConfigurationProperties properties=new MQConfigurationProperties();
        properties.setQueueManager("QM1");
        properties.setUser("admin");
        properties.setPassword("passw0rd");
        properties.setChannel("DEV.ADMIN.SVRCONN");
        properties.setConnName("localhost(1414)");
        return properties;
    }
}
