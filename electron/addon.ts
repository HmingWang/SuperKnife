import bindings from 'bindings';


export class AddonService {

    private addon: any;

    constructor() {
        this.addon = bindings('addon');
    }

    encodeBase64(data: string, wrap: boolean): string {
        try {
            return this.addon.base64Encode(data, wrap);
        } catch (error) {
            console.error('Error encoding to Base64:', error);
            throw error;
        }
    }

    decodeBase64(data: string): string {
        try {
            return this.addon.base64Decode(data);
        } catch (error) {
            console.error('Error decoding from Base64:', error);
            throw error;
        }
    }

    verifyLogin(username: string, password: string) {
        try {
            const result = this.addon.verifyLogin(username, password);
            return {
                success: result,
                token: result ? result.token : undefined
            };
        } catch (error) {
            console.error('Error verifying login:', error);
            throw error;
        }
    }

}