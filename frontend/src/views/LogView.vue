<template>

    <n-grid x-gap="12" y-gap="12" :cols="2">
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 7">
            <NCard title="Show finance">
                <div>Show finance data.</div>
                
                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showShowFinanceModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
            
        </n-gi>
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 7">
            <NCard title="Report finance">
                <div>Report finance data.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showReportFinanceModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 7">
            <NCard title="Report employee">
                <div>Report employee data.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showReportEmployeeModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
        <n-gi v-if="loginStackStore.getCurrentUser().privilege >= 7">
            <NCard title="Show Log">
                <div>Show log data.</div>

                <template #action>
                    <NFlex justify="end">
                        <NButton type="primary" @click="showShowLogModal = true">Execute</NButton>
                    </NFlex>
                </template>
            </NCard>
        </n-gi>
    </n-grid>


    <NModal v-model:show="showShowFinanceModal">
        <n-card closable @close="showShowFinanceModal = false" style="width: 600px" title="ShowFinance"
            :bordered="false" size="huge" role="dialog" aria-modal="true">
        
            <NForm ref="showFinanceFormRef" :model="showFinanceForm">
                <NFormItem label="Count:" path="count">
                    <NInputNumber v-model:value="showFinanceForm.count" :min="1" :max="2147483647"
                        placeholder="Please enter count" />
                </NFormItem>
            </NForm>
            <NTag>Result:</NTag>
            <div ref="showFinanceResult">

            </div>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton type="primary" @click="handleShowFinanceSubmit" :disabled="!showFinanceButtonAvailable">
                        Query
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>
    
    <NModal v-model:show="showReportFinanceModal">
        <n-card closable @close="showReportFinanceModal = false" style="width: 600px" title="ReportFinance"
            :bordered="false" size="huge" role="dialog" aria-modal="true">
            <NTag>Result:</NTag>
            <div ref="reportFinanceResult">
            </div>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton type="primary" @click="handleReportFinanceSubmit" :disabled="!reportFinanceButtonAvailable">
                        Query
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>

    
    <NModal v-model:show="showReportEmployeeModal">
        <n-card closable @close="showReportEmployeeModal = false" style="width: 600px" title="ReportEmployee"
            :bordered="false" size="huge" role="dialog" aria-modal="true">
            <NTag>Result:</NTag>
            <div ref="reportEmployeeResult">
            </div>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton type="primary" @click="handleReportEmployeeSubmit" :disabled="!reportEmployeeButtonAvailable">
                        Query
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>

    

    <NModal v-model:show="showShowLogModal">
        <n-card closable @close="showShowLogModal = false" style="width: 600px" title="ShowLog"
            :bordered="false" size="huge" role="dialog" aria-modal="true">
            <NTag>Result:</NTag>
            <div ref="showLogResult">
            </div>
            <template #action>
                <NFlex class="login-card-footer" justify="end">
                    <NButton type="primary" @click="handleShowLogSubmit" :disabled="!showLogButtonAvailable">
                        Query
                    </NButton>
                </NFlex>
            </template>
        </n-card>
    </NModal>
</template>


<script setup lang="ts">
import { useLoginStackStore } from '@/stores/loginStack';
import { passwordValidator, usernameValidator, userIDValidator } from '@/stores/validators';
import {
    NCard,
    NGrid,
    NGi,
    NButton,
    NFlex,
    NModal,
    NText,
    NInputNumber,
    NFormItem,
    NForm,
    type FormInst,
    useMessage,
    NSelect,
    type FormItemRule,
    NTag
} from 'naive-ui';
import { computed, ref } from 'vue';
import axios, { type AxiosResponse, AxiosError } from 'axios';
import { API_BASE_URL } from '@/constants'
import { useTemplateRef } from 'vue'


const loginStackStore = useLoginStackStore();
const message = useMessage()

const showShowFinanceModal = ref(false);
const showFinanceButtonAvailable = ref(true)
const showFinanceResult = useTemplateRef('showFinanceResult')

const showFinanceFormRef = ref<FormInst | null>(null);
const showFinanceForm = ref({
    count: null as number | null,
});
const handleShowFinanceSubmit = async () => {
    console.log("handle showFinance")
    try {
        const headers: { [key: string]: string } = {
            'Content-Type': 'application/json',
        };
        const token = loginStackStore.getCurrentToken();
        if (token) {
            headers["Authorization"] = `Bearer ${token}`;
        }
        const params: { [key: string]: string } = {
            
        };
        if(showFinanceForm.value.count !== null){
            params['count'] = showFinanceForm.value.count.toString();
        }
        const response = await axios.get(
            API_BASE_URL + '/api/v1/logs/show_finance',
            { headers: headers, params: params });
            console.log(showFinanceResult.value);   
        showFinanceResult.value!.innerText = response.data;
        message.success("ShowFinance success.");
    } catch (error) {
        if (axios.isAxiosError(error)) {
            console.log(error)
            if (error.status === 401) {
                message.error("Token unavailable. Logging out current account.");
                loginStackStore.logout();
            }
            else if (error.status === 400) {
                message.error('Failed   ' + error.response?.data.message);
            }
            else {
                message.error('Network error.')
            }
        }
        else {
            message.error('Unknown error: ' + error);
        }
    }
}


const showReportFinanceModal = ref(false);
const reportFinanceButtonAvailable = ref(true)
const reportFinanceResult = useTemplateRef('reportFinanceResult')
const handleReportFinanceSubmit = async () => {
    try {
        const headers: { [key: string]: string } = {
            'Content-Type': 'application/json',
        };
        const token = loginStackStore.getCurrentToken();
        if (token) {
            headers["Authorization"] = `Bearer ${token}`;
        }
        const response = await axios.get(
            API_BASE_URL + '/api/v1/logs/report_finance',
            { headers: headers });
            console.log(reportFinanceResult.value);   
        reportFinanceResult.value!.innerText = response.data;
        message.success("ReportFinance success.");
    } catch (error) {
        if (axios.isAxiosError(error)) {
            console.log(error)
            if (error.status === 401) {
                message.error("Token unavailable. Logging out current account.");
                loginStackStore.logout();
            }
            else if (error.status === 400) {
                message.error('Failed   ' + error.response?.data.message);
            }
            else {
                message.error('Network error.')
            }
        }
        else {
            message.error('Unknown error: ' + error);
        }
    }
}



const showReportEmployeeModal = ref(false);
const reportEmployeeButtonAvailable = ref(true)
const reportEmployeeResult = useTemplateRef('reportEmployeeResult')
const handleReportEmployeeSubmit = async () => {
    try {
        const headers: { [key: string]: string } = {
            'Content-Type': 'application/json',
        };
        const token = loginStackStore.getCurrentToken();
        if (token) {
            headers["Authorization"] = `Bearer ${token}`;
        }
        const response = await axios.get(
            API_BASE_URL + '/api/v1/logs/report_employee',
            { headers: headers });
            console.log(reportEmployeeResult.value);   
        reportEmployeeResult.value!.innerText = response.data;
        message.success("ReportEmployee success.");
    } catch (error) {
        if (axios.isAxiosError(error)) {
            console.log(error)
            if (error.status === 401) {
                message.error("Token unavailable. Logging out current account.");
                loginStackStore.logout();
            }
            else if (error.status === 400) {
                message.error('Failed   ' + error.response?.data.message);
            }
            else {
                message.error('Network error.')
            }
        }
        else {
            message.error('Unknown error: ' + error);
        }
    }
}

const showShowLogModal = ref(false);
const showLogButtonAvailable = ref(true)
const showLogResult = useTemplateRef('showLogResult')
const handleShowLogSubmit = async () => {
    console.log("handle showLog")
    try {
        const headers: { [key: string]: string } = {
            'Content-Type': 'application/json',
        };
        const token = loginStackStore.getCurrentToken();
        if (token) {
            headers["Authorization"] = `Bearer ${token}`;
        }
        const response = await axios.get(
            API_BASE_URL + '/api/v1/logs/all',
            { headers: headers });
            console.log(showLogResult.value);   
        showLogResult.value!.innerText = response.data;
        message.success("ShowLog success.");
    } catch (error) {
        if (axios.isAxiosError(error)) {
            console.log(error)
            if (error.status === 401) {
                message.error("Token unavailable. Logging out current account.");
                loginStackStore.logout();
            }
            else if (error.status === 400) {
                message.error('Failed   ' + error.response?.data.message);
            }
            else {
                message.error('Network error.')
            }
        }
        else {
            message.error('Unknown error: ' + error);
        }
    }
}
</script>