import { defineStore } from 'pinia';

interface UserItem {
    userid: string;
    username: string;
    privilege: number;
    selected_id: number;
    token: string | null;
}

export const useLoginStackStore = defineStore('loginStack', {
    state: () => ({
        stack: [] as UserItem[],
    }),

    actions: {
        loadFromLocalStorage() {
            const storedStack = localStorage.getItem('loginStack');
            if (storedStack) {
                this.stack = JSON.parse(storedStack);
            }
        },
        saveToLocalStorage() {
            localStorage.setItem('loginStack', JSON.stringify(this.stack));
        },
        initGuest() {
            if (this.stack.length === 0 || this.stack[this.stack.length - 1]?.userid !== "<GUEST>") {
                this.stack.push({ userid: "<GUEST>", username: "No Account", privilege: 0, selected_id: 0, token: null });
            }
        },

        login(user: UserItem) {
            console.log("Loginstack::login");
            this.stack.unshift(user);
            this.saveToLocalStorage();
        },
        logout() {
            console.log("Loginstack::logout");
            if (this.stack.length > 0) {
                this.stack.shift();
                this.saveToLocalStorage();
            }
        },
        getCurrentUser():UserItem {
            return this.stack[0]!;
        },
        getCurrentToken(){
            return this.stack[0]!.token;
        }
    },
});